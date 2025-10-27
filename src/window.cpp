#include "window.hpp"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

void window_init() { glfwInit(); }

void Window::window_refresh_callback(GLFWwindow* window) {
    // recover the C++ object pointer
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) self->_update();
    // glFinish();
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width,
                                       int height) {
    glViewport(0, 0, width, height);

    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) self->_update();
}

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

GLuint Window::link_shader_program() {
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

void Window::create_shader_prorgam() {
    std::string vertSrc = loadFile("../shader/vertex.glsl");
    std::string fragSrc = loadFile("../shader/fragment.glsl");

    vert = compile_shader(GL_VERTEX_SHADER, vertSrc);
    frag = compile_shader(GL_FRAGMENT_SHADER, fragSrc);
    shader_program = link_shader_program();
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

    // init texture
    pixels.resize(width * height * 3);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

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
}

void Window::init_fullscreen_texture() {
    /*
    for (uint64_t i = 0; i < pixels.size(); i++) {
        if (i % 23 == 0) {
            pixels[i] = 255u;
        }
    }*/

    pixels.resize(width * height * 3);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // tight packing for byte RGB
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::_update() {
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // update texture data (same size, new pixels)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB,
                    GL_UNSIGNED_BYTE, pixels.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    // render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind program, texture and VAO
    glUseProgram(shader_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);

    // draw quad with triangle strip (no indices)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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

    renderer.init(-2, 1, -1, 1, 64);

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
    glfwSetWindowUserPointer(window, this);
    // glfwSetWindowRefreshCallback(window, window_refresh_callback);

    glewInit();

    // init

    create_shader_prorgam();
    create_fullscreen_quad();

    // renderer.render(width, height);
    init_fullscreen_texture();

    std::thread render_thread([this] { renderer.render(width, height); });
    // renderer.render(width, height);

    glUseProgram(shader_program);
    GLint loc = glGetUniformLocation(shader_program, "screenTexture");
    if (loc >= 0) glUniform1i(loc, 0);
    glUseProgram(0);

    while (!glfwWindowShouldClose(window)) {
        // input
        _update();
    }

    // cleanup
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader_program);

    glfwTerminate();
}
