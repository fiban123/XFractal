#pragma once

#include <gmp.h>
#include <mpfr.h>

#include "render_config.hpp"

template <typename MType>
struct FractalBounds {
    double d_x_min, d_x_max;
    double d_y_min, d_y_max;
    double i_width, i_height;

    MType x_min, x_max;
    MType y_min, y_max;
    MType width, height;
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
    inline static mpfr_prec_t prec = MPFR_PREC;
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
        mpfr_init_set(n, x, rnd);
    }
    inline static void init_set_i(mpfr_t n, int x) {
        mpfr_init_set_si(n, x, rnd);
    }
    inline static void init_set_d(mpfr_t n, double x) {
        mpfr_init_set_d(n, x, rnd);
    }
    inline static int get_i(mpfr_t n) { return mpfr_get_si(n, rnd); }
    inline static double get_d(mpfr_t n) { return mpfr_get_d(n, rnd); }

    inline static int cmp(mpfr_t a, mpfr_t b) { return mpfr_cmp(a, b); }
    inline static int cmp_i(mpfr_t a, int b) { return mpfr_cmp_si(a, b); }
    inline static int cmp_d(mpfr_t a, double b) { return mpfr_cmp_d(a, b); }

    inline static void clear(mpfr_t n) { mpfr_clear(n); }
};
