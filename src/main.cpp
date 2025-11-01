#include "render_config.hpp"
#include "window.hpp"

int main() {
    window_init();
    Window window(START_WINDOW_X, START_WINDOW_Y);
    window.init(START_WINDOW_X, START_WINDOW_Y);

    window.start();
}
