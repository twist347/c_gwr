#include "gwr_vertex_buffer.h"
#include "gwr_log.h"

#include <stdio.h>
#include <stdlib.h>

#define VB_LOG(level, msg, ...)    GWR_log((level), "[VERTEX BUFFER]: " msg, ##__VA_ARGS__)

struct GWR_vertex_buffer_t {
    GLuint id;
    GLsizeiptr size;
    GLenum usage;
};

static int check_created_size(GLenum target, GLsizeiptr expected);

GWR_vertex_buffer_t *GWR_vertex_buffer_create(const void *data, GLsizeiptr size, GLenum usage) {
    if (size <= 0) {
        VB_LOG(GWR_LOG_ERROR, "invalid buffer size: %ld", size);
        return NULL;
    }

    GWR_vertex_buffer_t *buffer = malloc(sizeof(GWR_vertex_buffer_t));
    if (!buffer) {
        VB_LOG(GWR_LOG_ERROR, "failed to allocate memory");
        return NULL;
    }

    buffer->id = 0;
    buffer->size = 0;
    buffer->usage = usage;

    glGenBuffers(1, &buffer->id);
    if (!buffer->id) {
        VB_LOG(GWR_LOG_ERROR, "failed to glGenBuffers");
        free(buffer);
        return NULL;
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    const int ok = check_created_size(GL_ARRAY_BUFFER, size);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!ok) {
        VB_LOG(GWR_LOG_ERROR, "glBufferData failed to allocate %ld bytes", size);
        glDeleteBuffers(1, &buffer->id);
        free(buffer);
        return NULL;
    }

    buffer->size = size;

    return buffer;
}

void GWR_vertex_buffer_destroy(GWR_vertex_buffer_t *buffer) {
    if (buffer) {
        if (buffer->id) {
            glDeleteBuffers(1, &buffer->id);
            buffer->id = 0;
        }
        free(buffer);
        buffer = NULL;
    }
}

void GWR_vertex_buffer_bind(const GWR_vertex_buffer_t *buffer) {
    if (buffer && buffer->id) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    }
}

void GWR_vertex_buffer_unbind(void) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GWR_vertex_buffer_set_data(GWR_vertex_buffer_t *buffer, const void *data, GLsizeiptr size) {
    if (buffer && buffer->id && data && size > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
        glBufferData(GL_ARRAY_BUFFER, size, data, buffer->usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        buffer->size = size;
    }
}

GLuint GWR_vertex_buffer_get_id(const GWR_vertex_buffer_t *buffer) {
    return buffer ? buffer->id : 0;
}

GLsizeiptr GWR_vertex_buffer_get_size(const GWR_vertex_buffer_t *buffer) {
    return buffer ? buffer->size : 0;
}

static int check_created_size(GLenum target, GLsizeiptr expected) {
    GLint64 actual = 0;
    glGetBufferParameteri64v(target, GL_BUFFER_SIZE, &actual);
    return actual == expected;
}
