#include "internal/gwr_vertex_buffer.h"
#include "internal/gwr_log.h"
#include "internal/gwr_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define VB_LOG(level, msg, ...)    GWR_log((level), "[VERTEX BUFFER]: " msg, ##__VA_ARGS__)

struct GWR_vertex_buffer_t {
    GLuint id;
    GLsizeiptr size;
    GLenum usage;
};

static bool check_created_size_bound(GLenum target, GLsizeiptr expected);
static bool check_created_size_named(GLuint id, GLsizeiptr expected);

static bool wrapper_create_buffer(GWR_vertex_buffer_t *vbo, const void *data, GLsizeiptr size, GLenum usage);
static void wrapper_set_data(GWR_vertex_buffer_t *vbo, const void *data, GLsizeiptr size);

GWR_vertex_buffer_t *GWR_vertex_buffer_create(const void *data, GLsizeiptr size, GLenum usage) {
    assert(size > 0);

    GWR_vertex_buffer_t *vbo = malloc(sizeof(GWR_vertex_buffer_t));
    if (!vbo) {
        VB_LOG(GWR_LOG_ERROR, "failed to allocate memory");
        return NULL;
    }

    vbo->id = 0;
    vbo->size = 0;
    vbo->usage = usage;

    if (!wrapper_create_buffer(vbo, data, size, usage)) {
        free(vbo);
        return NULL;
    }

    vbo->size = size;
    return vbo;
}

void GWR_vertex_buffer_destroy(GWR_vertex_buffer_t *vbo) {
    assert(vbo);
    assert(vbo->id);

    glDeleteBuffers(1, &vbo->id);
    vbo->id = 0;

    free(vbo);
}

void GWR_vertex_buffer_bind(const GWR_vertex_buffer_t *vbo) {
    assert(vbo);
    assert(vbo->id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
}

void GWR_vertex_buffer_unbind(void) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GWR_vertex_buffer_set_data(GWR_vertex_buffer_t *vbo, const void *data, GLsizeiptr size) {
    assert(vbo);
    assert(vbo->id);

    wrapper_set_data(vbo, data, size);

    vbo->size = size;
}

GLuint GWR_vertex_buffer_get_id(const GWR_vertex_buffer_t *vbo) {
    assert(vbo);
    assert(vbo->id);

    return vbo->id;
}

GLsizeiptr GWR_vertex_buffer_get_size(const GWR_vertex_buffer_t *vbo) {
    assert(vbo);

    return vbo->size;
}

GLenum GWR_vertex_buffer_get_usage(const GWR_vertex_buffer_t *vbo) {
    assert(vbo);

    return vbo->usage;
}

static bool check_created_size_bound(GLenum target, GLsizeiptr expected) {
    GLint64 actual = 0;
    glGetBufferParameteri64v(target, GL_BUFFER_SIZE, &actual);
    return actual == expected;
}

static bool check_created_size_named(GLuint id, GLsizeiptr expected) {
    GLint64 actual = 0;
    glGetNamedBufferParameteri64v(id, GL_BUFFER_SIZE, &actual);
    return actual == expected;
}

static bool wrapper_create_buffer(GWR_vertex_buffer_t *vbo, const void *data, GLsizeiptr size, GLenum usage) {
    assert(vbo);

#if GWR_USE_DSA

    vbo->id = 0;

    glCreateBuffers(1, &vbo->id);
    if (!vbo->id) {
        VB_LOG(GWR_LOG_ERROR, "glCreateBuffers returned 0");
        return false;
    }
    glNamedBufferData(vbo->id, size, data, usage);
    const bool ok = check_created_size_named(vbo->id, size);
    if (!ok) {
        VB_LOG(GWR_LOG_ERROR, "glNamedBufferData failed to allocate %td bytes", size);
        glDeleteBuffers(1, &vbo->id);
        vbo->id = 0;
        return false;
    }

#else
    glGenBuffers(1, &vbo->id);
    if (!vbo->id) {
        VB_LOG(GWR_LOG_ERROR, "glGenBuffers failed");
        return false;
    }

    GLint prev = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);

    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    const bool ok = check_created_size_bound(GL_ARRAY_BUFFER, size);
    glBindBuffer(GL_ARRAY_BUFFER, prev);

    if (!ok) {
        VB_LOG(GWR_LOG_ERROR, "glBufferData failed to allocate %td bytes", size);
        glDeleteBuffers(1, &vbo->id);
        vbo->id = 0;
        return false;
    }
#endif

    return true;
}

static void wrapper_set_data(GWR_vertex_buffer_t *vbo, const void *data, GLsizeiptr size) {
#if GWR_USE_DSA
    glNamedBufferData(vbo->id, size, data, vbo->usage);
#else
    GLint prev = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);

    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, vbo->usage);
    glBindBuffer(GL_ARRAY_BUFFER, prev);
#endif
}