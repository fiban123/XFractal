#include <iostream>

#include "render.hpp"
#include "window.hpp"

int main() {
    window_init();

    Window window(3000, 2000);

    window.init(3000, 2000);

    window.start();
}
