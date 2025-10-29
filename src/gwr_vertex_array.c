#include "internal/gwr_vertex_array.h"
#include "internal/gwr_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VA_LOG(level, msg, ...)    GWR_log((level), "[VERTEX ARRAY]: " msg, ##__VA_ARGS__)

struct GWR_vertex_array_t {
    GLuint id;
};

static void bind_vao_and_vbo(const GWR_vertex_array_t *vao, const GWR_vertex_buffer_t *vbo);

GWR_vertex_array_t *GWR_vertex_array_create(void) {
    GWR_vertex_array_t *array = malloc(sizeof(GWR_vertex_array_t));
    if (!array) {
        VA_LOG(GWR_LOG_ERROR, "failed to allocate memory");
        return NULL;
    }
    array->id = 0;

    glGenVertexArrays(1, &array->id);
    if (!array->id) {
        VA_LOG(GWR_LOG_ERROR, "failed to create vertex array");
        free(array);
        return NULL;
    }
    return array;
}

void GWR_vertex_array_destroy(GWR_vertex_array_t *vao) {
    assert(vao);
    assert(vao->id);

    glDeleteVertexArrays(1, &vao->id);
    vao->id = 0;

    free(vao);
    vao = NULL;
}

void GWR_vertex_array_bind(const GWR_vertex_array_t *vao) {
    assert(vao);
    assert(vao->id);

    glBindVertexArray(vao->id);
}

void GWR_vertex_array_unbind(void) {
    glBindVertexArray(0);
}

void GWR_vertex_array_set_element_buffer(const GWR_vertex_array_t *vao, const GWR_element_buffer_t *ebo) {
    assert(vao);
    assert(vao->id);
    assert(ebo);

    glBindVertexArray(vao->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GWR_element_buffer_get_id(ebo));
    glBindVertexArray(0);
}

GLuint GWR_vertex_array_get_id(const GWR_vertex_array_t *vao) {
    assert(vao);
    assert(vao->id);

    return vao->id;
}

void GWR_vertex_array_enable_attrib(GLuint idx) {
    glEnableVertexAttribArray(idx);
}

void GWR_vertex_array_disable_attrib(GLuint idx) {
    glDisableVertexAttribArray(idx);
}

void GWR_vertex_array_attrib_pointerf(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint idx, GLint size, GLenum type, GLboolean normalized,
    GLsizei stride, const void *pointer
) {
    assert(vao);
    assert(vbo);

    bind_vao_and_vbo(vao, vbo);
    glVertexAttribPointer(idx, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(idx);
}

void GWR_vertex_array_attrib_pointeri(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint idx, GLint size, GLenum type,
    GLsizei stride, const void *pointer
) {
    assert(vao);
    assert(vbo);

    bind_vao_and_vbo(vao, vbo);
    glVertexAttribIPointer(idx, size, type, stride, pointer);
    glEnableVertexAttribArray(idx);
}

void GWR_vertex_array_attrib_pointerl(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint idx, GLint size, GLenum type,
    GLsizei stride, const void *pointer
) {
    assert(vao);
    assert(vbo);

    bind_vao_and_vbo(vao, vbo);
    glVertexAttribLPointer(idx, size, type, stride, pointer);
    glEnableVertexAttribArray(idx);
}

static void bind_vao_and_vbo(const GWR_vertex_array_t *vao, const GWR_vertex_buffer_t *vbo) {
    assert(vao);
    assert(vbo);

    glBindVertexArray(vao->id);
    glBindBuffer(GL_ARRAY_BUFFER, GWR_vertex_buffer_get_id(vbo));
}
