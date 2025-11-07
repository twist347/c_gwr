#include "internal/gwr_window.h"
#include "internal/gwr_util.h"
#include "internal/gwr_log.h"
#include "internal/gwr_config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define WINDOW_LOG(level, msg, ...)    GWR_log((level), "[WINDOW]: " msg, ##__VA_ARGS__)
#define GLAD_LOG(level, msg, ...)      GWR_log((level), "[GLAD]: " msg, ##__VA_ARGS__)

struct GWR_window_t {
    GLFWwindow *handle;
};

// helper funcs

static void require_opengl_version_or_die(int req_major, int req_minor);

static void framebuffer_size_callback(GLFWwindow *handle, int width, int height);

static bool init_glad(void);

// public funcs

GWR_window_t *GWR_window_create(int width, int height, const char *title) {
    assert(width);
    assert(height);
    assert(title);

    if (!glfwInit()) {
        WINDOW_LOG(GWR_LOG_ERROR, "failed to initialize GLFW");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GWR_OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GWR_OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!handle) {
        WINDOW_LOG(GWR_LOG_ERROR, "failed to create GLFW window");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(handle);

    if (!init_glad()) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        return NULL;
    }

    require_opengl_version_or_die(GWR_OPENGL_MAJOR_VERSION, GWR_OPENGL_MINOR_VERSION);

    glfwSwapInterval(1); // VSync

    int fbw = 0, fbh = 0;
    glfwGetFramebufferSize(handle, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);

    GWR_window_t *window = malloc(sizeof(GWR_window_t));
    if (!window) {
        WINDOW_LOG(GWR_LOG_ERROR, "failed to allocate memory for window");
        glfwDestroyWindow(handle);
        glfwTerminate();
        return NULL;
    }

    window->handle = handle;

    return window;
}

void GWR_window_destroy(GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    glfwDestroyWindow(window->handle);
    window->handle = NULL;

    free(window);

    glfwTerminate();
}

void *GWR_window_get_handle(const GWR_window_t *window) {
    return window ? window->handle : NULL;
}

void GWR_window_set_vsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

bool GWR_window_should_close(const GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    return glfwWindowShouldClose(window->handle);
}

void GWR_window_process_input(const GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    if (glfwGetKey(GWR_window_get_handle(window), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(GWR_window_get_handle(window), GLFW_TRUE);
    }
}

void GWR_window_swap_buffers(const GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    glfwSwapBuffers(window->handle);
}

void GWR_window_poll_events(void) {
    glfwPollEvents();
}

void GWR_window_set_clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void GWR_window_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT);
}

void GWR_window_set_current(GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    glfwMakeContextCurrent(window->handle);
}

int GWR_window_get_width(const GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    int width, height;
    glfwGetWindowSize(window->handle, &width, &height);
    return width;
}

int GWR_window_get_height(const GWR_window_t *window) {
    assert(window);
    assert(window->handle);

    int width, height;
    glfwGetWindowSize(window->handle, &width, &height);
    return height;
}

// helper funcs

static void require_opengl_version_or_die(int req_major, int req_minor) {
    GLint cur_major = 0, cur_minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &cur_major);
    glGetIntegerv(GL_MINOR_VERSION, &cur_minor);

    const bool ok = (cur_major > req_major) || (cur_major == req_major && cur_minor >= req_minor);

    if (!ok) {
        WINDOW_LOG(
            GWR_LOG_ERROR,
            "OpenGL version does not match required version: expected (%d, %d), current(%d, %d)",
            req_major, req_minor,
            cur_major, cur_minor
        );
        abort();
    }
}

static void framebuffer_size_callback(GLFWwindow *handle, int width, int height) {
    assert(handle);

    GWR_UNUSED(handle);
    glViewport(0, 0, width, height);
}

static bool init_glad(void) {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        GLAD_LOG(GWR_LOG_ERROR, "failed to initialize GLAD");
        return false;
    }
    return true;
}
