#pragma once

#include <stdbool.h>

#include "glad/glad.h"

typedef enum {
    GWR_SHADER_UNIFORM_INT = 0,
    GWR_SHADER_UNIFORM_INT_VEC2,
    GWR_SHADER_UNIFORM_INT_VEC3,
    GWR_SHADER_UNIFORM_INT_VEC4,

    GWR_SHADER_UNIFORM_UINT,
    GWR_SHADER_UNIFORM_UINT_VEC2,
    GWR_SHADER_UNIFORM_UINT_VEC3,
    GWR_SHADER_UNIFORM_UINT_VEC4,

    GWR_SHADER_UNIFORM_FLOAT,
    GWR_SHADER_UNIFORM_VEC2,
    GWR_SHADER_UNIFORM_VEC3,
    GWR_SHADER_UNIFORM_VEC4,

    GWR_SHADER_UNIFORM_MAT2,
    GWR_SHADER_UNIFORM_MAT3,
    GWR_SHADER_UNIFORM_MAT4,

    GWR_SHADER_UNIFORM_DOUBLE,
    GWR_SHADER_UNIFORM_DOUBLE_VEC2,
    GWR_SHADER_UNIFORM_DOUBLE_VEC3,
    GWR_SHADER_UNIFORM_DOUBLE_VEC4,

    GWR_SHADER_UNIFORM_DOUBLE_MAT2,
    GWR_SHADER_UNIFORM_DOUBLE_MAT3,
    GWR_SHADER_UNIFORM_DOUBLE_MAT4,

    GWR_SHADER_UNIFORM__COUNT
} GWR_shader_uniform_data_type_t;

typedef struct GWR_shader_t GWR_shader_t;

bool GWR_shader_is_valid(const GWR_shader_t *shader);

GLuint GWR_shader_compile_src(GLenum type, const char *src);
GLuint GWR_shader_compile_path(GLenum type, const char *path);

GWR_shader_t *GWR_shader_create(GLuint vertex_shader, GLuint fragment_shader);
GWR_shader_t *GWR_shader_create_src(const char *vertex_shader_src, const char *fragment_shader_src);
GWR_shader_t *GWR_shader_create_path(const char *vertex_shader_path, const char *fragment_shader_path);

void GWR_shader_destroy(GWR_shader_t *shader);
void GWR_shader_use(const GWR_shader_t *shader);
GLuint GWR_shader_get_id(const GWR_shader_t *shader);
GLint GWR_shader_get_uniform_loc(const GWR_shader_t *shader, const char *name);

void GWR_shader_set_val_loc(
    const GWR_shader_t *shader,
    GLint loc,
    const void *val,
    GWR_shader_uniform_data_type_t type
);
void GWR_shader_set_val_name(
    const GWR_shader_t *shader,
    const char *name,
    const void *val,
    GWR_shader_uniform_data_type_t type
);

void GWR_shader_set_val_loc_n(
    const GWR_shader_t *shader,
    GLint loc,
    const void *val,
    GWR_shader_uniform_data_type_t type,
    GLsizei n
);
void GWR_shader_set_val_name_n(
    const GWR_shader_t *shader,
    const char *name,
    const void *val,
    GWR_shader_uniform_data_type_t type,
    GLsizei n
);
