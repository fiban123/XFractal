#include <vector>

#include "math.hpp"

struct Renderer {
    std::vector<unsigned char> pixels;
    MathType type;

    FractalBounds<mpfr_t> mpfr_bounds;
    FractalBounds<double> double_bounds;

    constexpr static MPFRMathFuncs mpfr_math_funcs{};
    constexpr static DoubleMathFuncs double_math_funcs{};

    void update_size_bounds(int width, int height);
    void update_fractal_bounds(double x_min, double x_max, double y_min,
                               double y_max);

    void bound_zoom(int x_start, int y_start, int x_end, int y_end);

    void reize_pixels(int width, int height);
    void clear_pixels();

    void set_math_type(MathType type);
    void render_mandelbrot(int max_iter, int res, int n_threads);
};
