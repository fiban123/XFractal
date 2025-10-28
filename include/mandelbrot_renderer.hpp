
#include <iostream>
#include <vector>

#include "math.hpp"

template <typename MType, auto& M>
void _render_mandelbrot(FractalBounds<MType>& bounds, int res, int n_threads,
                        int max_iter, std::vector<unsigned char>& pixels) {
    std::cout << "renderer called" << std::endl;
    MType tmp, zx2, zy2, nzx, nzy, tx, ty, cx, cy, zx, zy;
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

    // init constants
    MType two;
    M.init_set_i(two, 2);

    // compute scaling factors
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

    for (int y = 0; y < bounds.i_height; y++) {
        for (int x = 0; x < bounds.i_width; x++) {
            // map pixel coords to mandelbrot coords
            M.set_i(tx, x);
            M.set_i(ty, y);

            // cy = min_x + ty * dx
            M.mul(cx, tx, dx);
            M.add(cx, cx, bounds.x_min);
            // cy = min_y + ty * dy
            M.mul(cy, ty, dy);
            M.add(cy, cy, bounds.y_min);

            M.set_i(zx, 0);
            M.set_i(zy, 0);

            int iter = 0;
            for (; iter < (int)max_iter; iter++) {
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
            unsigned char color = (unsigned char)(255.0f * iter / max_iter);
            size_t idx = (y * bounds.i_width + x) * 3;
            pixels[idx + 0] = color;
            pixels[idx + 1] = color;
            pixels[idx + 2] = color;
        }
        std::cout << "c" << y << "\n";
    }
}
