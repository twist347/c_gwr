#include "gwr_vertex_array.h"
#include "gwr_log.h"

#include <stdio.h>
#include <stdlib.h>

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

void GWR_vertex_array_destroy(GWR_vertex_array_t *array) {
    if (array) {
        if (array->id) {
            glDeleteVertexArrays(1, &array->id);
            array->id = 0;
        }
        free(array);
    }
}

void GWR_vertex_array_bind(const GWR_vertex_array_t *array) {
    if (array && array->id) {
        glBindVertexArray(array->id);
    }
}

void GWR_vertex_array_unbind(void) {
    glBindVertexArray(0);
}

GLuint GWR_vertex_array_get_id(const GWR_vertex_array_t *array) {
    return array ? array->id : 0;
}

void GWR_vertex_array_enable_attrib(GLuint index) {
    glEnableVertexAttribArray(index);
}

void GWR_vertex_array_disable_attrib(GLuint index) {
    glDisableVertexAttribArray(index);
}

void GWR_vertex_array_set_divisor(GLuint index, GLuint divisor) {
    glVertexAttribDivisor(index, divisor);
}

void GWR_vertex_array_attrib_pointerf(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint index, GLint size, GLenum type, GLboolean normalized,
    GLsizei stride, const void *pointer
) {
    if (!vao || !vbo) {
        VA_LOG(GWR_LOG_ERROR, "attrib_pointerf: null vao or vbo");
        return;
    }
    bind_vao_and_vbo(vao, vbo);
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(index);
}

void GWR_vertex_array_attrib_pointeri(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint index, GLint size, GLenum type,
    GLsizei stride, const void *pointer
) {
    if (!vao || !vbo) {
        VA_LOG(GWR_LOG_ERROR, "attrib_pointeri: null vao or vbo");
        return;
    }
    bind_vao_and_vbo(vao, vbo);
    glVertexAttribIPointer(index, size, type, stride, pointer);
    glEnableVertexAttribArray(index);
}

void GWR_vertex_array_attrib_pointerl(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint index, GLint size, GLenum type,
    GLsizei stride, const void *pointer
) {
    if (!vao || !vbo) {
        VA_LOG(GWR_LOG_ERROR, "attrib_pointerl: null vao or vbo");
        return;
    }
    bind_vao_and_vbo(vao, vbo);
    glVertexAttribLPointer(index, size, type, stride, pointer);
    glEnableVertexAttribArray(index);
}

static void bind_vao_and_vbo(const GWR_vertex_array_t *vao, const GWR_vertex_buffer_t *vbo) {
    glBindVertexArray(vao ? vao->id : 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo ? GWR_vertex_buffer_get_id(vbo) : 0);
}
