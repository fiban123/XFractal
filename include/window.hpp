#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "render.hpp"

void window_init();

struct Window {
    int width, height;
    GLFWwindow* window;

    GLuint frag, vert, shader_program;
    GLuint texture;
    GLuint vao, vbo, ebo;

    std::vector<unsigned char> pixels;
    MandelBrotRenderer renderer;

    void init(int _width, int _height);
    void start();

    // callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width,
                                          int height);

    static void key_callback(GLFWwindow* window, int key, int scancode,
                             int action, int mods);

    static void mouse_button_callback(GLFWwindow* window, int button,
                                      int action, int mods);

    // shader stuff
    static std::string loadFile(const std::string& path);
    static GLuint compile_shader(GLenum type, const std::string& src);
    GLuint link_shader_program();
    void create_shader_prorgam();

    // vertice stuff
    void create_fullscreen_quad();

    // rendering stuff
    void _update();
    void init_fullscreen_texture();

    Window(int _width, int _height)
        : width(_width), height(_height), renderer(pixels) {}
};
