#include <iostream>

#include "render.hpp"
#include "window.hpp"

int main() {
    window_init();

    Window window(4000, 2000);

    window.init(4000, 2000);

    window.start();
}
