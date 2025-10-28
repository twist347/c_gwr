#include <stdlib.h>
#include <stddef.h>

#include "gwr.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE "03_texture_triangle example"

#define BG_COLOR GWR_WHITE

#define TEXTURE_PATH "textures/img2.png"

#define VERTEX_SHADER_PATH "shaders/shader.vert"
#define FRAGMENT_SHADER_PATH "shaders/shader.frag"

typedef struct {
    GLfloat pos[3];
    GLfloat color[3];
    GLfloat tex_coord[2];
} vertex_t;

int main() {
    int exit_code = EXIT_SUCCESS;
    GWR_window_t *window = NULL;
    GWR_shader_t *shader = NULL;
    GWR_vertex_buffer_t *vbo = NULL;
    GWR_vertex_array_t *vao = NULL;
    GWR_texture_t *texture = NULL;

    window = GWR_window_create(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);

    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    GWR_info_print();

    const vertex_t vertices[] = {
        {.pos = {-0.5f, -0.5f, 0.f}, .color = {1.f, 0.f, 0.f}, .tex_coord = {0.f, 0.f}},
        {.pos = {0.5f, -0.5f, 0.f}, .color = {0.f, 1.f, 0.f}, .tex_coord = {1.f, 0.f}},
        {.pos = {0.0f, 0.5f, 0.f}, .color = {0.f, 0.f, 1.f}, .tex_coord = {0.5f, 1.f}},
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
        0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
        (void *) offsetof(vertex_t, pos)
    );
    GWR_vertex_array_attrib_pointerf(
        vao, vbo,
        1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
        (void *) offsetof(vertex_t, color)
    );
    GWR_vertex_array_attrib_pointerf(
        vao, vbo,
        2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
        (void *) offsetof(vertex_t, tex_coord)
    );

    shader = GWR_shader_create_path(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    if (!shader) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    const int val = 0;
    GWR_shader_set_val_name(shader, "tex_sampler", &val, GWR_SHADER_UNIFORM_INT);

    texture = GWR_texture_load(TEXTURE_PATH);
    if (!texture) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    GWR_window_set_clear_color(GWR_UNPACK_COLOR(BG_COLOR), 1.f);

    while (!GWR_window_should_close(window)) {
        GWR_window_process_input(window);
        GWR_window_clear();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GWR_texture_get_id(texture));

        GWR_shader_use(shader);
        GWR_vertex_array_bind(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        GWR_vertex_array_unbind();

        GWR_window_swap_buffers(window);
        GWR_window_poll_events();
    }

cleanup:
    if (texture) {
        GWR_texture_destroy(texture);
    }

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
