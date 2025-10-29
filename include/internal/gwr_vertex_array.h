#pragma once

#include "gwr_vertex_buffer.h"
#include "gwr_element_buffer.h"

typedef struct GWR_vertex_array_t GWR_vertex_array_t;

GWR_vertex_array_t *GWR_vertex_array_create(void);
void GWR_vertex_array_destroy(GWR_vertex_array_t *vao);

void GWR_vertex_array_bind(const GWR_vertex_array_t *vao);
void GWR_vertex_array_unbind(void);

void GWR_vertex_array_set_element_buffer(const GWR_vertex_array_t *vao, const GWR_element_buffer_t *ebo);

GLuint GWR_vertex_array_get_id(const GWR_vertex_array_t *vao);

void GWR_vertex_array_enable_attrib(GLuint idx);
void GWR_vertex_array_disable_attrib(GLuint idx);

void GWR_vertex_array_attrib_pointerf(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint idx, GLint size, GLenum type, GLboolean normalized,
    GLsizei stride, const void *pointer
);
void GWR_vertex_array_attrib_pointeri(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint idx, GLint size, GLenum type,
    GLsizei stride, const void *pointer
);
void GWR_vertex_array_attrib_pointerl(
    const GWR_vertex_array_t *vao,
    const GWR_vertex_buffer_t *vbo,
    GLuint idx, GLint size, GLenum type,
    GLsizei stride, const void *pointer
);
