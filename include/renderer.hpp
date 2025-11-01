#include <format>
#include <iostream>
#include <vector>

#include "math.hpp"

template <typename MType, MathFuncsConcept<MType> auto& M>
void arb_normalize_bounds(FractalBounds<MType>& bounds, MType* offset_x_out,
                          MType* offset_y_out) {
    MType center_x, center_y;
    MType two;

    M.init(center_x);
    M.init(center_y);
    M.init_set_i(two, 2);

    // find center of bounds
    // center = (max + min) / 2
    M.add(center_x, bounds.x_max, bounds.x_min);
    M.div(center_x, center_x, two);

    M.add(center_y, bounds.y_max, bounds.y_min);
    M.div(center_y, center_y, two);

    // center is the offset to apply
    if (offset_x_out) {
        M.set(*offset_x_out, center_x);
    }
    if (offset_y_out) {
        M.set(*offset_y_out, center_y);
    }

    // apply offsets
    // min = min - offset_#_out
    M.sub(bounds.x_min, bounds.x_min, center_x);
    M.sub(bounds.x_max, bounds.x_max, center_x);

    M.sub(bounds.y_min, bounds.y_min, center_y);
    M.sub(bounds.y_max, bounds.y_max, center_y);

    M.clear(center_x);
    M.clear(center_y);
    M.clear(two);
}

template <typename MType, MathFuncsConcept<MType> auto& M>
void arb_unnormalize_bounds(FractalBounds<MType>& bounds, MType& offset_x_src,
                            MType& offset_y_src) {
    // apply offsets in reverse

    // #_m## = #_m## + offset_#_src
    M.add(bounds.x_min, bounds.x_min, offset_x_src);
    M.add(bounds.x_max, bounds.x_max, offset_x_src);

    M.add(bounds.y_min, bounds.y_min, offset_y_src);
    M.add(bounds.y_max, bounds.y_max, offset_y_src);
}

template <typename MType, MathFuncsConcept<MType> auto& M>
void arb_move_bound_windowed(FractalBounds<MType>& bounds, int _wx, int _wy) {
    // normalize to range 0,1

    double _nwx = (double)_wx / (double)bounds.i_width;
    double _nwy = (double)_wy / (double)bounds.i_height;

    // convert to fractal coords
    MType nwx, nwy;
    MType fx, fy;
    MType zero, one;
    M.init_set_d(nwx, _nwx);
    M.init_set_d(nwy, _nwy);
    M.init_set_i(zero, 0);
    M.init_set_i(one, 1);
    M.init(fx);
    M.init(fy);

    map<MType, M>(fx, nwx, zero, one, bounds.r_x_min, bounds.r_x_max);
    map<MType, M>(fy, nwy, zero, one, bounds.r_y_min, bounds.r_y_max);

    arb_normalize_bounds<MType, M>(bounds, nullptr, nullptr);

    arb_unnormalize_bounds<MType, M>(bounds, fx, fy);

    std::cout << "rendered y: [" << M.get_d(bounds.r_y_min) << ", "
              << M.get_d(bounds.r_y_max) << "]\n";

    std::cout << "x: " << M.get_d(fx) << "y: " << M.get_d(fy) << std::endl;

    M.clear(nwx);
    M.clear(nwy);
    M.clear(fx);
    M.clear(fy);
    M.clear(zero);
    M.clear(one);
}

template <typename MType, MathFuncsConcept<MType> auto& M>
void arb_bound_zoom(FractalBounds<MType>& bounds, double _zoom_factor) {
    MType offset_x, offset_y;
    MType zoom_factor;

    M.init(offset_x);
    M.init(offset_y);
    M.init_set_d(zoom_factor, _zoom_factor);

    // normalize boubds
    arb_normalize_bounds<MType, M>(bounds, &offset_x, &offset_y);

    // apply zoom
    M.div(bounds.x_min, bounds.x_min, zoom_factor);
    M.div(bounds.x_max, bounds.x_max, zoom_factor);

    M.div(bounds.y_min, bounds.y_min, zoom_factor);
    M.div(bounds.y_max, bounds.y_max, zoom_factor);

    // unnormalize bounds
    arb_unnormalize_bounds<MType, M>(bounds, offset_x, offset_y);

    bounds.template update_aux<M>();

    // return;

    std::cout << std::format("x_min: {}, x_max: {}, y_min: {}, 66y_max: {}\n",
                             bounds.d_x_min, bounds.d_x_max, bounds.d_y_min,
                             bounds.d_y_max);

    M.clear(offset_x);
    M.clear(offset_y);
    M.clear(zoom_factor);
}

template <typename MType, MathFuncsConcept<MType> auto& M>
inline void get_windowed_bound_rect(FractalBounds<MType>& bounds, double& x1,
                                    double& y1, double& x2, double& y2) {
    MType neg_one;
    MType one;

    MType w_x_min, w_x_max;
    MType w_y_min, w_y_max;

    M.init(w_x_min);
    M.init(w_x_max);
    M.init(w_y_min);
    M.init(w_y_max);

    M.init_set_i(neg_one, -1);
    M.init_set_i(one, 1);

    map<MType, M>(w_x_min, bounds.x_min, bounds.r_x_min, bounds.r_x_max,
                  neg_one, one);
    map<MType, M>(w_x_max, bounds.x_max, bounds.r_x_min, bounds.r_x_max,
                  neg_one, one);

    map<MType, M>(w_y_min, bounds.y_min, bounds.r_y_min, bounds.r_y_max, one,
                  neg_one);
    map<MType, M>(w_y_max, bounds.y_max, bounds.r_y_min, bounds.r_y_max, one,
                  neg_one);

    x1 = M.get_d(w_x_min);
    y1 = M.get_d(w_y_min);
    x2 = M.get_d(w_x_max);
    y2 = M.get_d(w_y_max);

    M.clear(one);
    M.clear(neg_one);
    M.clear(w_x_min);
    M.clear(w_x_max);
    M.clear(w_y_min);
    M.clear(w_y_max);
}

struct Renderer {
    std::vector<unsigned char> pixels;
    MathType type;

    size_t iterations = 64;

    FractalBounds<mpfr_t> mpfr_bounds;
    FractalBounds<double> double_bounds;

    mpfr_t zoom_level;

    constexpr static MPFRMathFuncs mpfr_math_funcs{};
    constexpr static DoubleMathFuncs double_math_funcs{};

    void init_bounds();
    void set_window_size_i(int width, int height);
    void set_fractal_bounds_d(double x_min, double x_max, double y_min,
                              double y_max);

    void bound_zoom(double zoom_factor);
    void bound_move(int wx, int wy);
    void window_get_bounds(double& out_x_min, double& out_x_max,
                           double& out_y_min, double& out_y_max);

    void resize_pixels(int width, int height);
    void clear_pixels();

    void set_math_type(MathType type);
    void render_mandelbrot(int res, int n_threads);

    Renderer();
};
