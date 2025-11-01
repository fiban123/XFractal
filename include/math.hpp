#pragma once

#include <gmp.h>
#include <mpfr.h>

#include "render_config.hpp"

template <typename M, typename T>
concept MathFuncsConcept = requires(T a, T b, T c, int i, double d) {
    { M::init(a) };
    { M::add(a, b, c) };
    { M::sub(a, b, c) };
    { M::mul(a, b, c) };
    { M::div(a, b, c) };
    { M::set(a, b) };
    { M::set_i(a, i) };
    { M::set_d(a, d) };
    { M::init_set(a, b) };
    { M::init_set_i(a, i) };
    { M::init_set_d(a, d) };
    { M::get_i(a) };
    { M::get_d(a) };
    { M::cmp(a, b) };
    { M::cmp_i(a, i) };
    { M::cmp_d(a, d) };
    { M::clear(a) };
};

template <typename MType, MathFuncsConcept<MType> auto& M>
void map(MType& out, MType& x, MType& min_1, MType& max_1, MType& min_2,
         MType& max_2) {
    MType scaled_x;
    MType range_1;
    MType range_2;

    M.init(scaled_x);
    M.init(range_1);
    M.init(range_2);

    M.sub(scaled_x, x, min_1);
    M.sub(range_2, max_2, min_2);
    M.sub(range_1, max_1, min_1);

    M.mul(out, scaled_x, range_2);
    M.div(out, out, range_1);
    M.add(out, out, min_2);

    M.clear(scaled_x);
    M.clear(range_1);
    M.clear(range_2);
}

template <typename MType>
struct FractalBounds {
    double d_x_min, d_x_max;
    double d_y_min, d_y_max;
    int i_width, i_height;

    MType x_min, x_max;
    MType y_min, y_max;

    // rendered bounds
    MType r_x_min, r_x_max;
    MType r_y_min, r_y_max;

    MType width, height;

    template <MathFuncsConcept<MType> auto& M>
    void update_aux() {
        d_x_min = M.get_d(x_min);
        d_x_max = M.get_d(x_max);
        d_y_min = M.get_d(y_min);
        d_y_max = M.get_d(y_max);

        i_width = M.get_i(width);
        i_height = M.get_i(height);
    }

    void get_aux_bounds(double& _x_min, double& _x_max, double& _y_min,
                        double& _y_max, int& _width, int& _height) {
        _x_min = d_x_min;
        _x_max = d_x_max;
        _y_min = d_y_min;
        _y_max = d_y_max;
        _width = i_width;
        _height = i_height;
    }

    template <MathFuncsConcept<MType> auto& M>
    void update_rendered() {
        M.set(r_x_min, x_min);
        M.set(r_x_max, x_max);
        M.set(r_y_min, y_min);
        M.set(r_y_max, y_max);
    }

    template <MathFuncsConcept<MType> auto& M>
    void init() {
        M.init(x_min);
        M.init(x_max);
        M.init(y_min);
        M.init(y_max);
        M.init(r_x_min);
        M.init(r_x_max);
        M.init(r_y_min);
        M.init(r_y_max);
        M.init(width);
        M.init(height);
    }

    template <MathFuncsConcept<MType> auto& M>
    void set_bounds_d(double _x_min, double _x_max, double _y_min,
                      double _y_max) {
        M.set_d(x_min, _x_min);
        M.set_d(x_max, _x_max);
        M.set_d(y_min, _y_min);
        M.set_d(y_max, _y_max);
        M.set_d(r_x_min, _x_min);
        M.set_d(r_x_max, _x_max);
        M.set_d(r_y_min, _y_min);
        M.set_d(r_y_max, _y_max);
        d_x_min = _x_min;
        d_x_max = _x_max;
        d_y_min = _y_min;
        d_y_max = _y_max;
    }

    template <MathFuncsConcept<MType> auto& M>
    void set_sizes_i(int _width, int _height) {
        M.set_i(width, _width);
        M.set_i(height, _height);
        i_width = _width;
        i_height = _height;
    }
};

enum class MathType { DOUBLE, FLOAT, MPFR, MPQ };

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

struct MPFRMathFuncs {
    inline static mpfr_prec_t prec = START_MPFR_PREC;
    const static mpfr_rnd_t rnd = MPFR_RNDN;

    inline static void init(mpfr_t n) { mpfr_init2(n, prec); }

    inline static void add(mpfr_t n, mpfr_t a, mpfr_t b) {
        mpfr_add(n, a, b, rnd);
    }
    inline static void sub(mpfr_t n, mpfr_t a, mpfr_t b) {
        mpfr_sub(n, a, b, rnd);
    }
    inline static void mul(mpfr_t n, mpfr_t a, mpfr_t b) {
        mpfr_mul(n, a, b, rnd);
    }
    inline static void div(mpfr_t n, mpfr_t a, mpfr_t b) {
        mpfr_div(n, a, b, rnd);
    }

    inline static void set(mpfr_t n, mpfr_t x) { mpfr_set(n, x, rnd); }
    inline static void set_i(mpfr_t n, int x) { mpfr_set_si(n, x, rnd); }
    inline static void set_d(mpfr_t n, double x) { mpfr_set_d(n, x, rnd); }
    inline static void init_set(mpfr_t n, mpfr_t x) {
        init(n);
        mpfr_set(n, x, rnd);
    }
    inline static void init_set_i(mpfr_t n, int x) {
        init(n);
        mpfr_set_si(n, x, rnd);
    }
    inline static void init_set_d(mpfr_t n, double x) {
        init(n);
        mpfr_set_d(n, x, rnd);
    }
    inline static int get_i(mpfr_t n) { return mpfr_get_si(n, rnd); }
    inline static double get_d(mpfr_t n) { return mpfr_get_d(n, rnd); }

    inline static int cmp(mpfr_t a, mpfr_t b) { return mpfr_cmp(a, b); }
    inline static int cmp_i(mpfr_t a, int b) { return mpfr_cmp_si(a, b); }
    inline static int cmp_d(mpfr_t a, double b) { return mpfr_cmp_d(a, b); }

    inline static void clear(mpfr_t n) { mpfr_clear(n); }
};
