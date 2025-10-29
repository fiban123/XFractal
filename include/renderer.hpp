#include <vector>

#include "math.hpp"

template <typename MType, MathFuncsConcept<MType> auto& M>
void arb_bound_zoom(FractalBounds<MType>& bounds, int _wx, int _wy,
                    double _zoom_factor) {
    MType wx, wy, zoom_factor;
    MType ax_range, ay_range;
    MType nwx, nwy;
    MType inwx, inwy;
    MType fx, fy;
    MType new_range_x, new_range_y;
    MType one;

    M.init_set_i(wx, _wx);
    M.init_set_i(wy, _wy);
    M.init_set_d(zoom_factor, _zoom_factor);

    M.init(ax_range);
    M.init(ay_range);
    M.init(nwx);
    M.init(nwy);
    M.init(inwx);
    M.init(inwy);
    M.init(fx);
    M.init(fy);
    M.init(new_range_x);
    M.init(new_range_y);

    M.init_set_i(one, 1);

    // ax_range = x_max - x_min
    M.sub(ax_range, bounds.x_max, bounds.x_min);
    M.sub(ay_range, bounds.y_max, bounds.y_min);

    // normalize to window coords; nwx = wx / width
    M.div(nwx, wx, bounds.width);
    M.div(nwy, wy, bounds.height);

    M.sub(nwy, one, nwy);  // invert y

    // turn normalized coords into abs fractal coords
    // fx = x_min + nwx * ax_range
    M.mul(fx, nwx, ax_range);
    M.add(fx, fx, bounds.x_min);
    M.mul(fy, nwy, ay_range);
    M.add(fy, fy, bounds.y_min);

    // zoom
    // new_range_x = ax_range / zoom_factor
    M.div(new_range_x, ax_range, zoom_factor);
    M.div(new_range_y, ay_range, zoom_factor);

    // denormalize normalized coords
    // invert nwx, nwy
    M.sub(inwx, one, nwx);
    M.sub(inwy, one, nwy);

    // x_min = new_range_x * nwx
    M.mul(bounds.x_min, new_range_x, nwx);
    M.mul(bounds.x_max, new_range_x, inwx);
    M.mul(bounds.y_min, new_range_y, nwy);
    M.mul(bounds.y_max, new_range_y, inwy);
    // x_min = fx - x_min;
    M.sub(bounds.x_min, fx, bounds.x_min);
    M.add(bounds.x_max, fx, bounds.x_max);
    M.sub(bounds.y_min, fy, bounds.y_min);
    M.add(bounds.y_max, fy, bounds.y_max);

    bounds.template update_aux<M>();

    M.clear(ax_range);
    M.clear(ay_range);
    M.clear(nwx);
    M.clear(nwy);
    M.clear(inwx);
    M.clear(inwy);
    M.clear(fx);
    M.clear(fy);
    M.clear(new_range_x);
    M.clear(new_range_y);
}

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

    void bound_zoom(int x, int y, double zoom_factor);

    void reize_pixels(int width, int height);
    void clear_pixels();

    void set_math_type(MathType type);
    void render_mandelbrot(int max_iter, int res, int n_threads);
};
