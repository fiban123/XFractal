#include "window.hpp"

int main() {
    window_init();

    Window window(2560 / 2, 1706 / 2);

    window.init(2560 / 2, 1706 / 2);

    window.start();
}
