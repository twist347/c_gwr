#include <stdlib.h>
#include <stddef.h>

#include "gwr.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE "02_triangle_example"

#define BG_COLOR GWR_WHITE

const char *vertex_src =
        "#version 460 core\n"

        "layout (location = 0) in vec3 a_pos;\n"
        "layout (location = 1) in vec3 a_color;\n"

        "out vec3 color;\n"

        "void main() {\n"
        "    gl_Position = vec4(a_pos, 1.0);\n"
        "    color = a_color;\n"
        "}\n";

const char *fragment_src =
        "#version 460 core\n"

        "out vec4 frag_color;\n"

        "in vec3 color;\n"

        "void main() {\n"
        "    frag_color = vec4(color, 1.0);\n"
        "}\n";

typedef struct {
    GLfloat pos[3];
    GLfloat color[3];
} vertex_t;

int main() {
    int exit_code = EXIT_SUCCESS;
    GWR_window_t *window = NULL;
    GWR_shader_t *shader = NULL;
    GWR_vertex_buffer_t *vbo = NULL;
    GWR_vertex_array_t *vao = NULL;

    window = GWR_window_create(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);

    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    GWR_info_print();

    const vertex_t vertices[] = {
        {.pos = {-0.5f, -0.5f, 0.f}, .color = {1.f, 0.f, 0.f}}, // left
        {.pos = {0.5f, -0.5f, 0.f}, .color = {0.f, 1.f, 0.f}}, // right
        {.pos = {0.0f, 0.5f, 0.f}, .color = {0.f, 0.f, 1.f}} // top
    };

    vbo = GWR_vertex_buffer_create(vertices, sizeof(vertices), GL_STATIC_DRAW);
    if (!vbo) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    vao = GWR_vertex_array_create();
    if (!vao) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    GWR_vertex_array_attrib_pointerf(
        vao, vbo,
        0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        (void *) offsetof(vertex_t, pos)
    );

    GWR_vertex_array_attrib_pointerf(
        vao, vbo,
        1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        (void *) offsetof(vertex_t, color)
    );

    shader = GWR_shader_create_src(vertex_src, fragment_src);
    if (!shader) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    GWR_window_set_clear_color(GWR_UNPACK_COLOR(BG_COLOR), 1.f);

    while (!GWR_window_should_close(window)) {
        GWR_window_process_input(window);

        GWR_window_clear();

        GWR_shader_use(shader);
        GWR_vertex_array_bind(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        GWR_vertex_array_unbind();

        GWR_window_swap_buffers(window);
        GWR_window_poll_events();
    }

cleanup:
    if (vao) {
        GWR_vertex_array_destroy(vao);
    }

    if (vbo) {
        GWR_vertex_buffer_destroy(vbo);
    }

    if (shader) {
        GWR_shader_destroy(shader);
    }

    if (window) {
        GWR_window_destroy(window);
    }

    return exit_code;
}
