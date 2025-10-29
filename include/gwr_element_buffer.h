#pragma once

#include "glad/glad.h"

typedef struct GWR_element_buffer_t GWR_element_buffer_t;

GWR_element_buffer_t *GWR_element_buffer_create(const void *data, GLsizeiptr size, GLenum usage);
void GWR_element_buffer_destroy(GWR_element_buffer_t *ebo);

void GWR_element_buffer_bind(const GWR_element_buffer_t *ebo);
void GWR_element_buffer_unbind(void);

void GWR_element_buffer_set_data(GWR_element_buffer_t *ebo, const void *data, GLsizeiptr size);

GLuint GWR_element_buffer_get_id(const GWR_element_buffer_t *ebo);
GLsizeiptr GWR_element_buffer_get_size(const GWR_element_buffer_t *ebo);
GLsizei GWR_element_buffer_get_count(const GWR_element_buffer_t *ebo);
GLenum GWR_element_buffer_get_type(const GWR_element_buffer_t *ebo);
GLenum GWR_element_buffer_get_usage(const GWR_element_buffer_t *ebo);