#pragma once

#include "glad/glad.h"

typedef struct GWR_vertex_buffer_t GWR_vertex_buffer_t;

GWR_vertex_buffer_t *GWR_vertex_buffer_create(const void *data, GLsizeiptr size, GLenum usage);
void GWR_vertex_buffer_destroy(GWR_vertex_buffer_t *buffer);

void GWR_vertex_buffer_bind(const GWR_vertex_buffer_t *buffer);
void GWR_vertex_buffer_unbind(void);

void GWR_vertex_buffer_set_data(GWR_vertex_buffer_t *buffer, const void *data, GLsizeiptr size);

GLuint GWR_vertex_buffer_get_id(const GWR_vertex_buffer_t *buffer);
GLsizeiptr GWR_vertex_buffer_get_size(const GWR_vertex_buffer_t *buffer);
