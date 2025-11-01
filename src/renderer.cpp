#include "renderer.hpp"

#include <ostream>

#include "mandelbrot_renderer.hpp"
#include "math.hpp"
#include "thread_manager.hpp"

void Renderer::set_window_size_i(int width, int height) {
    mpfr_bounds.set_sizes_i<mpfr_math_funcs>(width, height);
    double_bounds.set_sizes_i<double_math_funcs>(width, height);
}

void Renderer::init_bounds() {
    mpfr_bounds.init<mpfr_math_funcs>();
    double_bounds.init<double_math_funcs>();
}

void Renderer::set_fractal_bounds_d(double x_min, double x_max, double y_min,
                                    double y_max) {
    mpfr_bounds.set_bounds_d<mpfr_math_funcs>(x_min, x_max, y_min, y_max);
    double_bounds.set_bounds_d<double_math_funcs>(x_min, x_max, y_min, y_max);
}

void Renderer::bound_zoom(double zoom_factor) {
    mpfr_mul_d(zoom_level, zoom_level, zoom_factor, MPFR_RNDN);

    arb_bound_zoom<mpfr_t, mpfr_math_funcs>(mpfr_bounds, zoom_factor);
    arb_bound_zoom<double, double_math_funcs>(double_bounds, zoom_factor);
}

void Renderer::bound_move(int wx, int wy) {
    arb_move_bound_windowed<mpfr_t, mpfr_math_funcs>(mpfr_bounds, wx, wy);
    arb_move_bound_windowed<double, double_math_funcs>(double_bounds, wx, wy);
}

void Renderer::window_get_bounds(double& x1, double& y1, double& x2,
                                 double& y2) {
    switch (type) {
        case MathType::DOUBLE: {
            get_windowed_bound_rect<double, double_math_funcs>(double_bounds,
                                                               x1, y1, x2, y2);
            break;
        }
        case MathType::FLOAT: {
            break;
        }
        case MathType::MPFR: {
            get_windowed_bound_rect<mpfr_t, mpfr_math_funcs>(mpfr_bounds, x1,
                                                             y1, x2, y2);
            break;
        }
        case MathType::MPQ: {
            break;
        }
    }
}

void Renderer::resize_pixels(int width, int height) {
    pixels.resize(width * height * 3);
}

void Renderer::set_math_type(MathType _type) { type = _type; }

void Renderer::render_mandelbrot(int res, int n_threads) {
    std::cout << "rendering mandelbrot...\n";
    std::cout << "current zoom level: ";
    mpfr_exp_t exp;
    char* buf = mpfr_get_str(NULL, &exp, 10, 5, zoom_level, MPFR_RNDN);
    std::cout << buf[0] << '.' << buf + 1 << "e+" << exp << std::endl;
    mpfr_free_str(buf);
    std::cout << "num iterations: " << iterations << std::endl;

    switch (type) {
        case MathType::DOUBLE: {
            _render_fractal<
                double, double_math_funcs,
                _mandelbrot_section_renderer<double, double_math_funcs> >(
                double_bounds, res, n_threads, iterations, pixels);
            break;
        }
        case MathType::FLOAT: {
            break;
        }
        case MathType::MPFR: {
            _render_fractal<
                mpfr_t, mpfr_math_funcs,
                _mandelbrot_section_renderer<mpfr_t, mpfr_math_funcs> >(
                mpfr_bounds, res, n_threads, iterations, pixels);
            break;
        }
        case MathType::MPQ: {
            break;
        }
    }
}

Renderer::Renderer() { mpfr_init_set_si(zoom_level, 1, MPFR_RNDN); }
