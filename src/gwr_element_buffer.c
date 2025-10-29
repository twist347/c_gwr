#include "internal/gwr_element_buffer.h"
#include "internal/gwr_log.h"

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define EBO_LOG(level, msg, ...)    GWR_log((level), "[ELEMENT BUFFER]:", msg, ##__VA_ARGS__)

static bool check_created_size(GLenum target, GLsizeiptr expected);

struct GWR_element_buffer_t {
    GLuint id;
    GLsizeiptr size;
    GLsizei count;
    GLenum type;
    GLenum usage;
};

GWR_element_buffer_t *GWR_element_buffer_create(const void *data, GLsizeiptr size, GLenum usage) {
    if (size <= 0) {
        EBO_LOG(GWR_LOG_ERROR, "invalid buffer size: %ld", size);
        return NULL;
    }

    GWR_element_buffer_t *buffer = malloc(sizeof(GWR_element_buffer_t));
    if (!buffer) {
        EBO_LOG(GWR_LOG_ERROR, "failed to allocate GWR_element_buffer_t");
        return NULL;
    }

    buffer->id = 0;
    buffer->size = 0;
    buffer->count = 0;
    buffer->type = GL_UNSIGNED_INT;
    buffer->usage = usage;

    glGenBuffers(1, &buffer->id);
    if (!buffer->id) {
        EBO_LOG(GWR_LOG_ERROR, "failed to glGenBuffers");
        free(buffer);
        return NULL;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    const int ok = check_created_size(GL_ELEMENT_ARRAY_BUFFER, size);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (!ok) {
        EBO_LOG(GWR_LOG_ERROR, "glBufferData failed to allocate %ld bytes", size);
        glDeleteBuffers(1, &buffer->id);
        free(buffer);
        return NULL;
    }

    buffer->size = size;
    buffer->count = size / sizeof(GLuint);
    buffer->type = GL_UNSIGNED_INT;

    return buffer;
}

void GWR_element_buffer_destroy(GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->id);

    glDeleteBuffers(1, &ebo->id);
    ebo->id = 0;

    free(ebo);
    ebo = NULL;
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
    assert(ebo->size);
    assert(ebo->type);
    assert(ebo->usage);
    assert(data);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, ebo->usage);

    if (check_created_size(GL_ELEMENT_ARRAY_BUFFER, size)) {
        ebo->size = size;
        ebo->count = size / sizeof(GLuint); // Обновляем счетчик
    } else {
        EBO_LOG(GWR_LOG_ERROR, "glBufferData failed to allocate %ld bytes", size);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint GWR_element_buffer_get_id(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->id);

    return ebo->id;
}

GLsizeiptr GWR_element_buffer_get_size(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->size);

    return ebo->size;
}

GLsizei GWR_element_buffer_get_count(const GWR_element_buffer_t *ebo) {
    assert(ebo);
    assert(ebo->count);

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

static bool check_created_size(GLenum target, GLsizeiptr expected) {
    GLint64 actual = 0;
    glGetBufferParameteri64v(target, GL_BUFFER_SIZE, &actual);
    return actual == expected;
}