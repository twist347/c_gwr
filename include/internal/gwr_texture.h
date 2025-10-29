#pragma once

#include "glad/glad.h"

typedef struct GWR_texture_t GWR_texture_t;

GWR_texture_t *GWR_texture_load(const char *path);
void GWR_texture_destroy(GWR_texture_t *texture);

GLuint GWR_texture_get_id(const GWR_texture_t *texture);
GLsizei GWR_texture_get_width(const GWR_texture_t *texture);
GLsizei GWR_texture_get_height(const GWR_texture_t *texture);
