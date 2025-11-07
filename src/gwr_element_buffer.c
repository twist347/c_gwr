#include "internal/gwr_element_buffer.h"
#include "internal/gwr_log.h"
#include "internal/gwr_config.h"

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define EBO_LOG(level, msg, ...)    GWR_log((level), "[ELEMENT BUFFER]: " msg, ##__VA_ARGS__)

static bool check_created_size_bound(GLenum target, GLsizeiptr expected);
static bool check_created_size_named(GLuint id, GLsizeiptr expected);

static bool wrapper_create_buffer(GWR_element_buffer_t *ebo, const void *data, GLsizeiptr size, GLenum usage);
static void wrapper_set_data(GWR_element_buffer_t* ebo, const void* data, GLsizeiptr size);

struct GWR_element_buffer_t {
    GLuint id;
    GLsizeiptr size;
    GLsizei count;
    GLenum type;
    GLenum usage;
};

GWR_element_buffer_t *GWR_element_buffer_create(const void *data, GLsizeiptr size, GLenum usage) {
    assert(size > 0);

    GWR_element_buffer_t *ebo = malloc(sizeof(GWR_element_buffer_t));
    if (!ebo) {
        EBO_LOG(GWR_LOG_ERROR, "failed to allocate GWR_element_buffer_t");
        return NULL;
    }

    ebo->id = 0;
    ebo->size = 0;
    ebo->count = 0;
    ebo->type = GL_UNSIGNED_INT;
    ebo->usage = usage;

    if (!wrapper_create_buffer(ebo, data, size, usage)) {
        free(ebo);
        return NULL;
    }

    ebo->size  = size;
    ebo->count = size / sizeof(GLuint);

    return ebo;
}

void GWR_element_buffer_destroy(GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->id);

    glDeleteBuffers(1, &ebo->id);
    ebo->id = 0;

    free(ebo);
}

void GWR_element_buffer_bind(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
}

void GWR_element_buffer_unbind(void) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GWR_element_buffer_set_data(GWR_element_buffer_t *ebo, const void *data, GLsizeiptr size) {
    assert(ebo);
    assert(ebo->id);
    assert(ebo->type);
    assert(ebo->usage);

    wrapper_set_data(ebo, data, size);

    ebo->size  = size;
    ebo->count = size / sizeof(GLuint);
}

GLuint GWR_element_buffer_get_id(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->id);

    return ebo->id;
}

GLsizeiptr GWR_element_buffer_get_size(const GWR_element_buffer_t *ebo) {
    assert(ebo);

    return ebo->size;
}

GLsizei GWR_element_buffer_get_count(const GWR_element_buffer_t *ebo) {
    assert(ebo);

    return ebo->count;
}

GLenum GWR_element_buffer_get_type(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->type);

    return ebo->type;
}

GLenum GWR_element_buffer_get_usage(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->usage);

    return ebo->usage;
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

static bool wrapper_create_buffer(GWR_element_buffer_t *ebo, const void *data, GLsizeiptr size, GLenum usage) {
#if GWR_USE_DSA
    glCreateBuffers(1, &ebo->id);
    if (!ebo->id) {
        EBO_LOG(GWR_LOG_ERROR, "glCreateBuffers returned 0");
        return false;
    }
    glNamedBufferData(ebo->id, size, data, usage);
    if (!check_created_size_named(ebo->id, size)) {
        EBO_LOG(GWR_LOG_ERROR, "glNamedBufferData failed to allocate %d bytes", size);
        glDeleteBuffers(1, &ebo->id);
        ebo->id = 0;
        return false;
    }
#else
    glGenBuffers(1, &ebo->id);
    if (!ebo->id) {
        EBO_LOG(GWR_LOG_ERROR, "glGenBuffers failed");
        return false;
    }

    GLint prev_vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prev_vao);

    glBindVertexArray(0);

    GLint vao0_prev_ebo = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &vao0_prev_ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    const bool ok = check_created_size_bound(GL_ELEMENT_ARRAY_BUFFER, size);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)vao0_prev_ebo);

    glBindVertexArray(prev_vao);

    if (!ok) {
        EBO_LOG(GWR_LOG_ERROR, "glBufferData failed to allocate %td bytes", (ptrdiff_t)size);
        glDeleteBuffers(1, &ebo->id);
        ebo->id = 0;
        return false;
    }
#endif
    return true;
}

static void wrapper_set_data(GWR_element_buffer_t* ebo, const void* data, GLsizeiptr size) {
#if GWR_USE_DSA
    glNamedBufferData(ebo->id, size, data, ebo->usage);
#else
    GLint prev_vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prev_vao);

    glBindVertexArray(0);

    GLint vao0_prev_ebo = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &vao0_prev_ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, ebo->usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao0_prev_ebo);
    glBindVertexArray((GLuint)prev_vao);
#endif
}
