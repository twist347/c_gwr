#include "internal/gwr_texture.h"
#include "internal/gwr_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TEXTURE_LOG(level, msg, ...)    GWR_log((level), "[TEXTURE]: " msg, ##__VA_ARGS__)

struct GWR_texture_t {
    GLuint id;
    GLsizei width;
    GLsizei height;
};

static void set_default_params(void);

static bool choose_formats(int channels, GLenum *internal_format, GLenum *format);

static GLuint create_gl_texture_from_pixels(int width, int height, int channels, const unsigned char *pixels);

static GLuint texture_load(const char *path, int *w, int *h);

GWR_texture_t *GWR_texture_load(const char *path) {
    assert(path);

    GLsizei width, height;
    const GLuint id = texture_load(path, &width, &height);
    if (!id) {
        return NULL;
    }

    GWR_texture_t *tex = malloc(sizeof(GWR_texture_t));
    if (!tex) {
        glDeleteTextures(1, &id);
        return NULL;
    }
    tex->id = id;
    tex->width = width;
    tex->height = height;
    return tex;
}

void GWR_texture_destroy(GWR_texture_t *texture) {
    assert(texture);
    assert(texture->id);

    glDeleteTextures(1, &texture->id);
    texture->id = 0;

    free(texture);
}

GLuint GWR_texture_get_id(const GWR_texture_t *texture) {
    assert(texture);

    return texture->id;
}

GLsizei GWR_texture_get_width(const GWR_texture_t *texture) {
    assert(texture);

    return texture->width;
}

GLsizei GWR_texture_get_height(const GWR_texture_t *texture) {
    assert(texture);

    return texture->height;
}

static void set_default_params(void) {
    // repeat + trilinear (with mipmaps)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

static bool choose_formats(int channels, GLenum *internal_format, GLenum *format) {
    switch (channels) {
        case 1: *internal_format = GL_R8;
            *format = GL_RED;
            return true;
        case 2: *internal_format = GL_RG8;
            *format = GL_RG;
            return true;
        case 3: *internal_format = GL_RGB8;
            *format = GL_RGB;
            return true;
        case 4: *internal_format = GL_RGBA8;
            *format = GL_RGBA;
            return true;
        default:
            return false;
    }
}

static GLuint create_gl_texture_from_pixels(int width, int height, int channels, const unsigned char *pixels) {
    assert(pixels);
    assert(width);
    assert(height);

    GLenum internal_format = 0, format = 0;
    if (!choose_formats(channels, &internal_format, &format)) {
        TEXTURE_LOG(GWR_LOG_ERROR, "unsupported channel count: %d", channels);
        return 0;
    }

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    if (!texture_id) {
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    set_default_params();

    // Ensure tight rows for arbitrary widths
    GLint prev_unpack = 0;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev_unpack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D, 0, (GLint) internal_format, width, height,
        0, format, GL_UNSIGNED_BYTE, pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    // restore state and unbind
    glPixelStorei(GL_UNPACK_ALIGNMENT, prev_unpack);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_id;
}

static GLuint texture_load(const char *path, int *w, int *h) {
    assert(path);

    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        TEXTURE_LOG(GWR_LOG_ERROR, "failed to load image '%s'", path);
        return 0;
    }
    const GLuint id = create_gl_texture_from_pixels(width, height, channels, data);
    *w = width;
    *h = height;
    stbi_image_free(data);
    return id;
}
