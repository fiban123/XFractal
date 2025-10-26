#pragma once

#include <gmp.h>
#include <mpfr.h>

#include <vector>

struct DoubleMathFuncs {
    inline static void init(double& n) { (void)n; }

    inline static void add(double& n, double& a, double& b) { n = a + b; }
    inline static void sub(double& n, double& a, double& b) { n = a - b; }
    inline static void mul(double& n, double& a, double& b) { n = a * b; }
    inline static void div(double& n, double& a, double& b) { n = a / b; }

    inline static void set(double& n, double& x) { n = x; }
    inline static void set_i(double& n, int x) { n = (double)x; }
    inline static void set_d(double& n, double x) { n = x; };
    inline static void init_set(double& n, double& x) { n = x; }
    inline static void init_set_i(double& n, int x) { set_i(n, x); }
    inline static void init_set_d(double& n, double x) { set_d(n, x); }
    inline static int get_i(double& n) { return (int)n; }
    inline static double get_d(double& n) { return n; }

    // returns positive value if (a > b), zero if (a == b) and negative value if
    // (a < b)
    inline static int cmp(double& a, double& b) {
        return (a == b ? 0 : (a > b ? 1 : -1));
    }
    inline static int cmp_i(double& a, int b) {
        return ((int)a == b ? 0 : ((int)a > b ? 1 : -1));
    }
    inline static int cmp_d(double& a, double b) { return cmp(a, b); }

    inline static void clear(double& a) { (void)a; }
};

using MType = double;
using MathFuncs = DoubleMathFuncs;

struct RendererBase {
    std::vector<unsigned char>& pixels;

    constexpr static MathFuncs M{};

    MType width, height;

    MType x_min, x_max;
    MType y_min, y_max;

    void _init_bounds(int _x_min, int _x_max, int _y_min, int _y_max);

    virtual void render(int _width, int _height) = 0;

    virtual ~RendererBase() = default;

    RendererBase(std::vector<unsigned char>& _pixels) : pixels(_pixels) {}
};

struct MandelBrotRenderer : RendererBase {
    unsigned int max_iter;

    virtual void init(int _x_min, int _x_max, int _y_min, int _y_max,
                      int _max_iter);
    virtual void render(int _width, int _height);

    MandelBrotRenderer(std::vector<unsigned char>& _pixels)
        : RendererBase(_pixels) {}
};
