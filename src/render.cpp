#include "render.hpp"

#include <iostream>
void RendererBase::_init_bounds(int _x_min, int _x_max, int _y_min,
                                int _y_max) {
    M.init_set_i(x_min, _x_min);
    M.init_set_i(x_max, _x_max);
    M.init_set_i(y_min, _y_min);
    M.init_set_i(y_max, _y_max);
}

void MandelBrotRenderer::init(int _x_min, int _x_max, int _y_min, int _y_max,
                              int _max_iter) {
    _init_bounds(_x_min, _x_max, _y_min, _y_max);

    max_iter = _max_iter;
}

void MandelBrotRenderer::render(int _width, int _height) {
    M.init_set_i(width, _width);
    M.init_set_i(height, _height);

    MType tmp, zx2, zy2, nzx, nzy, tx, ty, cx, cy, zx, zy;
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
    M.sub(dx, x_max, x_min);
    M.div(dx, dx, width);
    // dy = (y_max - y_min) / height
    M.sub(dy, y_max, y_min);
    M.div(dy, dy, height);

    pixels.resize(_width * _height * 3);

    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            // map pixel coords to mandelbrot coords
            M.set_i(tx, x);
            M.set_i(ty, y);

            // cy = min_x + ty * dx
            M.mul(cx, tx, dx);
            M.add(cx, cx, x_min);
            // cy = min_y + ty * dy
            M.mul(cy, ty, dy);
            M.add(cy, cy, y_min);

            M.set_i(zx, 0);
            M.set_i(zy, 0);

            int iter = 0;
            for (; iter < (int)max_iter; iter++) {
                // iterrate

                // calculate zx^2 and zy^2
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
            size_t idx = (y * _width + x) * 3;
            pixels[idx + 0] = color;
            pixels[idx + 1] = color;
            pixels[idx + 2] = color;
        }
        std::cout << "c" << y << "\n";
    }
}
