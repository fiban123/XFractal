#include "renderer.hpp"

#include "mandelbrot_renderer.hpp"
#include "math.hpp"

void Renderer::update_size_bounds(int width, int height) {
    mpfr_math_funcs.init_set_i(mpfr_bounds.width, width);
    mpfr_math_funcs.init_set_i(mpfr_bounds.height, height);

    double_math_funcs.init_set_i(double_bounds.width, width);
    double_math_funcs.init_set_i(double_bounds.height, height);

    mpfr_bounds.i_width = width;
    mpfr_bounds.i_height = height;

    double_bounds.i_width = width;
    double_bounds.i_height = height;
}

void Renderer::update_fractal_bounds(double x_min, double x_max, double y_min,
                                     double y_max) {
    mpfr_math_funcs.init_set_d(mpfr_bounds.x_max, x_max);
    mpfr_math_funcs.init_set_d(mpfr_bounds.x_min, x_min);
    mpfr_math_funcs.init_set_d(mpfr_bounds.y_max, y_max);
    mpfr_math_funcs.init_set_d(mpfr_bounds.y_min, y_min);

    double_math_funcs.init_set_d(double_bounds.x_max, x_max);
    double_math_funcs.init_set_d(double_bounds.x_min, x_min);
    double_math_funcs.init_set_d(double_bounds.y_max, y_max);
    double_math_funcs.init_set_d(double_bounds.y_min, y_min);

    mpfr_bounds.d_x_max = x_max;
    mpfr_bounds.d_x_min = x_min;
    mpfr_bounds.d_y_max = y_max;
    mpfr_bounds.d_y_min = y_min;

    double_bounds.d_x_max = x_max;
    double_bounds.d_x_min = x_min;
    double_bounds.d_y_max = y_max;
    double_bounds.d_y_min = y_min;
}

void Renderer::bound_zoom(int wx, int wy, double zoom_factor) {
    arb_bound_zoom<mpfr_t, mpfr_math_funcs>(mpfr_bounds, wx, wy, zoom_factor);
    arb_bound_zoom<double, double_math_funcs>(double_bounds, wx, wy,
                                              zoom_factor);
}

void Renderer::reize_pixels(int width, int height) {
    pixels.resize(width * height * 3);
}

void Renderer::set_math_type(MathType _type) { type = _type; }

void Renderer::render_mandelbrot(int max_iter, int res, int n_threads) {
    switch (type) {
        case MathType::DOUBLE: {
            _render_mandelbrot<double, double_math_funcs>(
                double_bounds, res, n_threads, max_iter, pixels);
            break;
        }
        case MathType::FLOAT: {
            break;
        }
        case MathType::MPFR: {
            _render_mandelbrot<mpfr_t, mpfr_math_funcs>(
                mpfr_bounds, res, n_threads, max_iter, pixels);
            break;
        }
        case MathType::MPQ: {
            break;
        }
    }
}
