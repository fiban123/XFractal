
#include <iostream>
#include <vector>

#include "math.hpp"

template <typename MType, MathFuncsConcept<MType> auto& M>
void _mandelbrot_section_renderer(int iterations, MType& x_min, MType& y_min,
                                  int width, int height, int start_x, int end_x,
                                  int start_y, int end_y, MType& dx, MType& dy,
                                  std::vector<unsigned char>& pixels) {
    MType tmp, zx2, zy2, nzx, nzy, tx, ty, cx, cy, zx, zy, zero;
    M.init(tmp);
    M.init(zx2);
    M.init(zy2);
    M.init(nzx);
    M.init(nzy);
    M.init(tx);
    M.init(ty);
    M.init(cx);
    M.init(cy);
    M.init(zx);
    M.init(zy);
    M.init_set_i(zero, 0);

    // init constants
    MType two;
    M.init_set_i(two, 2);

    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            // map pixel coords to mandelbrot coords
            M.set_i(tx, x);
            M.set_i(ty, height - y);

            // cy = min_x + ty * dx
            M.mul(cx, tx, dx);
            M.add(cx, cx, x_min);
            // cy = min_y + ty * dy
            M.mul(cy, ty, dy);
            M.add(cy, cy, y_min);

            M.set_i(zx, 0);
            M.set_i(zy, 0);

            int iter = 0;
            for (; iter < iterations; iter++) {
                // iterrate

                // calculate zx^2 and zy^const 2
                M.mul(zx2, zx, zx);
                M.mul(zy2, zy, zy);

                // check if magnitude > 4
                M.add(tmp, zx2, zy2);
                if (M.cmp_i(tmp, 4) > 0) {
                    break;
                }

                // Zn^2

                // z(n+1)x = z(n)x^2 - z(n)y^2
                M.sub(nzx, zx2, zy2);
                // z(n+1)y = 2 * z(n)x * z(n)y
                M.mul(nzy, two, zx);
                M.mul(nzy, nzy, zy);

                // + C
                M.add(nzx, nzx, cx);
                M.add(nzy, nzy, cy);

                // update z
                M.set(zx, nzx);
                M.set(zy, nzy);
            }

            // std::cout << x << " " << y << " " << iter << "\n";
            //  map iter to color
            unsigned char color = (unsigned char)(255.0f * iter / iterations);
            size_t idx = (y * width + x) * 3;
            pixels[idx + 0] = color;
            pixels[idx + 1] = color;
            pixels[idx + 2] = color;
        }
        // std::cout << "c" << y << "\n";
    }
}
