#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "math.hpp"

template <typename MType, MathFuncsConcept<MType> auto& M>
using SectionRendererFunc = void (*)(int a, MType& b, MType&, int, int, int,
                                     int, int, int, MType&, MType&,
                                     std::vector<unsigned char>&);

// bounds of a section of a fractal that a section_renderer can compute
struct ComputeSection {
    int start_x, end_x, start_y, end_y;
    // std::atomic<bool> computed = false;
};

// pool of computbe sections
template <typename MType, MathFuncsConcept<MType> auto& M>
struct ComputePool {
    std::vector<ComputeSection> sections;
    std::atomic<int> computed = 0;

    // generate compute bounds based on fractal bounds
    void create_pool_section_bounds(FractalBounds<MType>& bounds,
                                    int _x_sections, int _y_sections) {
        double section_width = (double)bounds.i_width / (double)_x_sections;
        double section_height = (double)bounds.i_height / (double)_y_sections;

        sections.resize(_x_sections * _y_sections);

        for (int _y = 0; _y < _y_sections; _y++) {
            for (int _x = 0; _x < _x_sections; _x++) {
                ComputeSection& section = sections[_y * _x_sections + _x];

                section.start_x = _x * section_width;
                section.start_y = _y * section_height;

                section.end_x = (_x + 1) * section_width;
                section.end_y = (_y + 1) * section_height;
            }
        }
    }

    bool get_new_section(int& out) {
        int index = computed.fetch_add(1);

        if (index >= (int)sections.size()) return false;  // no more sections

        out = index;
        return true;
    }
};

template <typename MType, MathFuncsConcept<MType> auto& M,
          SectionRendererFunc<MType, M> section_renderer>
void _render_thread(int max_iter, FractalBounds<MType>& bounds,
                    ComputePool<MType, M>& pool, MType& dx, MType& dy,
                    std::vector<unsigned char>& pixels) {
    while (1) {
        // get section index
        int index;
        if (!pool.get_new_section(index)) {
            return;
        }

        ComputeSection& section = pool.sections[index];

        section_renderer(max_iter, bounds.x_min, bounds.y_min, bounds.i_width,
                         bounds.i_height, section.start_x, section.end_x,
                         section.start_y, section.end_y, dx, dy, pixels);
    }
}

template <typename MType, MathFuncsConcept<MType> auto& M,
          SectionRendererFunc<MType, M> section_renderer>
void _render_fractal(FractalBounds<MType>& bounds, int res, int n_threads,
                     int max_iter, std::vector<unsigned char>& pixels) {
    std::cout << "renderer called" << std::endl;

    // precompute constant for converting pixel-coords to fractal coord (may be
    // slow bc of division)
    MType dx, dy;
    M.init(dx);
    M.init(dy);
    // dx = (x_max - x_min) / width
    M.sub(dx, bounds.x_max, bounds.x_min);
    M.div(dx, dx, bounds.width);
    // dy = (y_max - y_min) / height
    M.sub(dy, bounds.y_max, bounds.y_min);
    M.div(dy, dy, bounds.height);

    pixels.resize(bounds.i_width * bounds.i_height * 3);

    bounds.template update_rendered<M>();

    ComputePool<MType, M> pool;
    pool.create_pool_section_bounds(bounds, 10, 10);

    std::vector<std::thread> threads;

    for (int x = 0; x < n_threads; x++) {
        threads.emplace_back([&, max_iter] {
            _render_thread<MType, M, section_renderer>(max_iter, bounds, pool,
                                                       dx, dy, pixels);
        });
    }
    for (std::thread& thread : threads) {
        thread.join();
    }
}
