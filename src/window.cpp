#include "window.hpp"

#include <GLFW/glfw3.h>

#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "render_config.hpp"

void window_init() { glfwInit(); }

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action,
                          int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
            double factor;
            if (key == GLFW_KEY_W) {
                factor = 1.5;
            } else if (key == GLFW_KEY_S) {
                factor = 1. / 1.5;
            }
            self->renderer.bound_zoom(factor);
            self->update_bound_preview_rect();
        }

        else if (key == GLFW_KEY_ENTER) {
            std::thread renderer_thread([self] {
                self->renderer.render_mandelbrot(
                    1, std::thread::hardware_concurrency());
            });
            renderer_thread.detach();
        }

        else if (key == GLFW_KEY_1) {
            self->renderer.iterations += 64;
        }
    }
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width,
                                       int height) {
    glViewport(0, 0, width, height);

    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) self->_update();
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action,
                                   int mods) {}

std::string Window::loadFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) throw std::runtime_error("Failed to open file: " + path);
    std::string s;
    ifs.seekg(0, std::ios::end);
    s.resize((size_t)ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    ifs.read(&s[0], s.size());
    return s;
}

GLuint Window::compile_shader(GLenum type, const std::string& src) {
    GLuint sh = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(sh, 1, &cstr, nullptr);
    glCompileShader(sh);

    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        glDeleteShader(sh);
        throw std::runtime_error("Shader compile error:\n" + log);
    }
    return sh;
}

GLuint Window::link_shader_program(GLuint vert, GLuint frag) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        glDeleteProgram(prog);
        throw std::runtime_error("Program link error:\n" + log);
    }
    return prog;
}

void Window::create_shader_prorgam(GLuint& program, std::string vert_path,
                                   std::string frag_path) {
    std::string vertSrc = loadFile(vert_path);
    std::string fragSrc = loadFile(frag_path);

    GLint vert = compile_shader(GL_VERTEX_SHADER, vertSrc);
    GLint frag = compile_shader(GL_FRAGMENT_SHADER, fragSrc);
    program = link_shader_program(vert, frag);
    // shaders can be deleted after linking
    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Window::create_fullscreen_quad() {
    // Triangle strip order: BL, BR, TL, TR
    float verts[] = {
        // pos.x pos.y   u   v
        -1.0f, -1.0f, 0.0f, 0.0f,  // BL
        1.0f,  -1.0f, 1.0f, 0.0f,  // BR
        -1.0f, 1.0f,  0.0f, 1.0f,  // TL
        1.0f,  1.0f,  1.0f, 1.0f   // TR
    };
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Window::update_bound_preview_rect() {
    double x1, y1, x2, y2;
    renderer.window_get_bounds(x1, y1, x2, y2);

    /*
    std::cout << std::format("x1: {}, y1: {}, x2: {}, y2: {}\n", x1, y1, x2,
                             y2);*/

    vertices = {
        // BR
        (float)x2,
        (float)y1,
        0.f,
        // TL
        (float)x1,
        (float)y1,
        0.f,
        // BL
        (float)x1,
        (float)y2,
        0.f,
        // BR
        (float)x2,
        (float)y2,
        0.f,
        // BR
        (float)x2,
        (float)y1,
        0.f,
    };

    glBindVertexArray(vert_vao);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
}

void Window::init_fullscreen_texture() {
    /*
    for (uint64_t i = 0; i < pixels.size(); i++) {
        if (i % 23 == 0) {
            pixels[i] = 255u;
        }
    }*/

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // tight packing for byte RGB
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, renderer.pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::init_gl_objects() {
    create_shader_prorgam(tex_shader_program, "../shader/tex_vertex.glsl",
                          "../shader/tex_fragment.glsl");
    create_shader_prorgam(vert_shader_program, "../shader/vert_vertex.glsl",
                          "../shader/vert_fragment.glsl");

    // tex
    glGenVertexArrays(1, &tex_vao);
    glGenBuffers(1, &tex_vbo);

    glBindVertexArray(tex_vao);
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);

    constexpr GLsizei STRIDE = 4 * sizeof(float);
    // position (location = 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
    glEnableVertexAttribArray(0);
    // texcoord (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE,
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vert
    glGenVertexArrays(1, &vert_vao);
    glGenBuffers(1, &vert_vbo);

    glBindVertexArray(vert_vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Window::_update() {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        // renderer.bound_move(());

        renderer.bound_move((int)x, (int)y);
    }
    update_bound_preview_rect();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw texture layer

    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // update texture data (same size, new pixels)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB,
                    GL_UNSIGNED_BYTE, renderer.pixels.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    // bind program, texture and VAO
    glUseProgram(tex_shader_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(tex_vao);

    // draw quad with triangle strip (no indices)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw vert layer
    glUseProgram(vert_shader_program);
    glBindVertexArray(vert_vao);
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

    // cleanup binds for clarity (not strictly required every frame)
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    // glfw stuff
    glfwSwapBuffers(window);
    glfwPollEvents();
    // glfwGetWindowSize(window, &width, &height);
}

void Window::init(int _width, int _height) {
    width = _width;
    height = _height;

    renderer.init_bounds();

    renderer.set_window_size_i(width, height);
    renderer.set_fractal_bounds_d(-2.0, 1.0, 0.0, 2.0);
    renderer.set_math_type(MathType::MPFR);
    renderer.resize_pixels(width, height);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::start() {
    window = glfwCreateWindow(width, height, "hi", NULL, NULL);

    if (window == NULL) {
        throw std::runtime_error("failed to initialize window");
    }
    glfwMakeContextCurrent(window);

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glfwSetWindowRefreshCallback(window, window_refresh_callback);

    glewInit();

    // init

    // generate vao, vbos
    init_gl_objects();

    create_fullscreen_quad();

    update_bound_preview_rect();

    init_fullscreen_texture();

    // renderer.render_mandelbrot(1, 1);

    glUseProgram(tex_shader_program);
    GLint loc = glGetUniformLocation(tex_shader_program, "screenTexture");
    if (loc >= 0) glUniform1i(loc, 0);
    glUseProgram(0);

    while (!glfwWindowShouldClose(window)) {
        // input
        _update();
    }

    // cleanup
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &tex_vbo);
    glDeleteVertexArrays(1, &tex_vao);
    glDeleteProgram(tex_shader_program);

    glfwTerminate();
}
