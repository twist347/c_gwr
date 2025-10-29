#include "internal/gwr_shader.h"
#include "internal/gwr_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define SHADER_LOG(level, msg, ...)    GWR_log((level), "[SHADER]: " msg, ##__VA_ARGS__)

struct GWR_shader_t {
    GLuint id;
};

// helper funcs decls

static void wrapper_gl_prog_uniform1iv(GLuint program, GLint location, GLsizei count, const GLint *value);
static void wrapper_gl_prog_uniform2iv(GLuint program, GLint location, GLsizei count, const GLint *value);
static void wrapper_gl_prog_uniform3iv(GLuint program, GLint location, GLsizei count, const GLint *value);
static void wrapper_gl_prog_uniform4iv(GLuint program, GLint location, GLsizei count, const GLint *value);

static void wrapper_gl_prog_uniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
static void wrapper_gl_prog_uniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
static void wrapper_gl_prog_uniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);
static void wrapper_gl_prog_uniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint *value);

static void wrapper_gl_prog_uniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
static void wrapper_gl_prog_uniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
static void wrapper_gl_prog_uniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
static void wrapper_gl_prog_uniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);

static void wrapper_gl_prog_uniform_mat_2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static void wrapper_gl_prog_uniform_mat_3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static void wrapper_gl_prog_uniform_mat_4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

static void wrapper_gl_prog_uniform1dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
static void wrapper_gl_prog_uniform2dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
static void wrapper_gl_prog_uniform3dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);
static void wrapper_gl_prog_uniform4dv(GLuint program, GLint location, GLsizei count, const GLdouble *value);

static void wrapper_gl_prog_uniform_mat_2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
static void wrapper_gl_prog_uniform_mat_3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
static void wrapper_gl_prog_uniform_mat_4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);

static void set_uniform_n(GLuint shader_id, GLint loc_id, const void *val, GWR_shader_uniform_data_type_t type, GLsizei n);

static GLboolean check_compile_errors(GLuint shader_id, const char *type);

static GLboolean check_link_errors(GLuint program);

static char *read_from_text_file(const char *path, size_t *out_size);

// public API

bool GWR_shader_is_valid(const GWR_shader_t *shader) {
    return shader && shader->id > 0;
}

GLuint GWR_shader_compile_src(GLenum type, const char *src) {
    assert(src);

    const GLuint shader = glCreateShader(type);
    if (!shader) {
        SHADER_LOG(GWR_LOG_ERROR, "glCreateShader failed");
        return 0;
    }
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    const char *type_name = "SHADER";
    switch (type) {
        case GL_VERTEX_SHADER: type_name = "VERTEX";
            break;
        case GL_FRAGMENT_SHADER: type_name = "FRAGMENT";
            break;
#ifdef GL_GEOMETRY_SHADER
        case GL_GEOMETRY_SHADER: type_name = "GEOMETRY";
            break;
#endif
        default:
            break;
    }

    if (!check_compile_errors(shader, type_name)) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint GWR_shader_compile_path(GLenum type, const char *path) {
    assert(path);

    size_t sz = 0;
    char *src = read_from_text_file(path, &sz);
    if (!src) {
        SHADER_LOG(GWR_LOG_ERROR, "read failed: '%s'", path);
        return 0;
    }

    const GLuint id = GWR_shader_compile_src(type, src);
    if (!id) {
        SHADER_LOG(GWR_LOG_ERROR, "compile failed: '%s'", path);
    }
    free(src);
    return id;
}

GWR_shader_t *GWR_shader_create(GLuint vertex_shader, GLuint fragment_shader) {
    assert(vertex_shader);
    assert(fragment_shader);

    GWR_shader_t *shader = malloc(sizeof(GWR_shader_t));
    if (!shader) {
        SHADER_LOG(GWR_LOG_ERROR, "failed to allocate shader_t");
        return NULL;
    }

    const GLuint program = glCreateProgram();
    if (program == 0) {
        SHADER_LOG(GWR_LOG_ERROR, "glCreateProgram failed");
        free(shader);
        return NULL;
    }

    shader->id = program;

    glAttachShader(shader->id, vertex_shader);
    glAttachShader(shader->id, fragment_shader);
    glLinkProgram(shader->id);

    glDetachShader(shader->id, vertex_shader);
    glDetachShader(shader->id, fragment_shader);

    if (!check_link_errors(shader->id)) {
        glDeleteProgram(shader->id);
        free(shader);
        return NULL;
    }

    return shader;
}

GWR_shader_t *GWR_shader_create_src(const char *vertex_shader_src, const char *fragment_shader_src) {
    assert(vertex_shader_src);
    assert(fragment_shader_src);

    const GLuint vertex_shader = GWR_shader_compile_src(GL_VERTEX_SHADER, vertex_shader_src);
    if (vertex_shader == 0) {
        return NULL;
    }

    const GLuint fragment_shader = GWR_shader_compile_src(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return NULL;
    }

    GWR_shader_t *shader = GWR_shader_create(vertex_shader, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader;
}

GWR_shader_t *GWR_shader_create_path(const char *vertex_shader_path, const char *fragment_shader_path) {
    assert(vertex_shader_path);
    assert(fragment_shader_path);

    const GLuint vertex_shader = GWR_shader_compile_path(GL_VERTEX_SHADER, vertex_shader_path);
    if (vertex_shader == 0) {
        return NULL;
    }

    const GLuint fragment_shader = GWR_shader_compile_path(GL_FRAGMENT_SHADER, fragment_shader_path);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return NULL;
    }

    GWR_shader_t *prog = GWR_shader_create(vertex_shader, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return prog;
}

void GWR_shader_destroy(GWR_shader_t *shader) {
    assert(shader);
    assert(shader->id);

    glDeleteProgram(shader->id);
    shader->id = 0;

    free(shader);
    shader = NULL;
}

void GWR_shader_use(const GWR_shader_t *shader) {
    assert(shader);
    assert(shader->id);

    glUseProgram(shader->id);
}

GLuint GWR_shader_get_id(const GWR_shader_t *shader) {
    assert(shader);
    assert(shader->id);

    return shader->id;
}

GLint GWR_shader_get_uniform_loc(const GWR_shader_t *shader, const char *name) {
    if (!shader || !shader->id || !name) {
        return -1;
    }
    return glGetUniformLocation(shader->id, name);
}

void GWR_shader_set_val_loc(
    const GWR_shader_t *shader,
    GLint loc,
    const void *val,
    GWR_shader_uniform_data_type_t type
) {
    GWR_shader_set_val_loc_n(shader, loc, val, type, 1);
}

void GWR_shader_set_val_name(
    const GWR_shader_t *shader,
    const char *name,
    const void *val,
    GWR_shader_uniform_data_type_t type
) {
    GWR_shader_set_val_name_n(shader, name, val, type, 1);
}

void GWR_shader_set_val_loc_n(
    const GWR_shader_t *shader,
    GLint loc,
    const void *val,
    GWR_shader_uniform_data_type_t type,
    GLsizei n
) {
    set_uniform_n(shader->id, loc, val, type, n);
}

void GWR_shader_set_val_name_n(
    const GWR_shader_t *shader,
    const char *name,
    const void *val,
    GWR_shader_uniform_data_type_t type,
    GLsizei n
) {
    assert(shader);
    assert(shader->id);

    const GLint loc = glGetUniformLocation(shader->id, name);
    if (loc < 0) {
        SHADER_LOG(GWR_LOG_WARNING, "uniform '%s' not found", name);
        // return;
    }
    GWR_shader_set_val_loc_n(shader, loc, val, type, n);
}

// helpers

static void wrapper_gl_prog_uniform1iv(GLuint program, GLint location, GLsizei count, const GLint *value) {
    assert(glProgramUniform1iv);

    glProgramUniform1iv(program, location, count, value);
}

static void wrapper_gl_prog_uniform2iv(GLuint program, GLint location, GLsizei count, const GLint *value) {
    assert(glProgramUniform2iv);

    glProgramUniform2iv(program, location, count, value);
}
static void wrapper_gl_prog_uniform3iv(GLuint program, GLint location, GLsizei count, const GLint *value) {
    assert(glProgramUniform3iv);

    glProgramUniform3iv(program, location, count, value);
}
static void wrapper_gl_prog_uniform4iv(GLuint program, GLint location, GLsizei count, const GLint *value) {
    assert(glProgramUniform4iv);

    glProgramUniform4iv(program, location, count, value);
}

static void wrapper_gl_prog_uniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint *value) {
    assert(glProgramUniform1uiv);

    glProgramUniform1uiv(program, location, count, value);
}
static void wrapper_gl_prog_uniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint *value) {
    assert(glProgramUniform2uiv);

    glProgramUniform2uiv(program, location, count, value);
}

static void wrapper_gl_prog_uniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint *value) {
    assert(glProgramUniform3uiv);

    glProgramUniform3uiv(program, location, count, value);
}

static void wrapper_gl_prog_uniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint *value) {
    assert(glProgramUniform4uiv);

    glProgramUniform4uiv(program, location, count, value);
}

static void wrapper_gl_prog_uniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat *value) {
    assert(glProgramUniform1fv);

    glProgramUniform1fv(program, location, count, value);
}

static void wrapper_gl_prog_uniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat *value) {
    assert(glProgramUniform2fv);

    glProgramUniform2fv(program, location, count, value);
}

static void wrapper_gl_prog_uniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat *value) {
    assert(glProgramUniform3fv);

    glProgramUniform3fv(program, location, count, value);
}

static void wrapper_gl_prog_uniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value) {
    assert(glProgramUniform4fv);

    glProgramUniform4fv(program, location, count, value);
}

static void wrapper_gl_prog_uniform_mat_2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    assert(glProgramUniformMatrix2fv);

    glProgramUniformMatrix2fv(program, location, count, transpose, value);
}

static void wrapper_gl_prog_uniform_mat_3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    assert(glProgramUniformMatrix3fv);

    glProgramUniformMatrix3fv(program, location, count, transpose, value);
}

static void wrapper_gl_prog_uniform_mat_4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    assert(glProgramUniformMatrix4fv);

    glProgramUniformMatrix4fv(program, location, count, transpose, value);
}

static void wrapper_gl_prog_uniform1dv(GLuint program, GLint location, GLsizei count, const GLdouble *value) {
    assert(glProgramUniform1dv);

    glProgramUniform1dv(program, location, count, value);
}

static void wrapper_gl_prog_uniform2dv(GLuint program, GLint location, GLsizei count, const GLdouble *value) {
    assert(glProgramUniform2dv);

    glProgramUniform2dv(program, location, count, value);
}

static void wrapper_gl_prog_uniform3dv(GLuint program, GLint location, GLsizei count, const GLdouble *value) {
    assert(glProgramUniform3dv);

    glProgramUniform3dv(program, location, count, value);
}

static void wrapper_gl_prog_uniform4dv(GLuint program, GLint location, GLsizei count, const GLdouble *value) {
    assert(glProgramUniform4dv);

    glProgramUniform4dv(program, location, count, value);
}

static void wrapper_gl_prog_uniform_mat_2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value) {
    assert(glProgramUniformMatrix2dv);

    glProgramUniformMatrix2dv(program, location, count, transpose, value);
}

static void wrapper_gl_prog_uniform_mat_3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value) {
    assert(glProgramUniformMatrix3dv);

    glProgramUniformMatrix3dv(program, location, count, transpose, value);
}

static void wrapper_gl_prog_uniform_mat_4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value) {
    assert(glProgramUniformMatrix4dv);

    glProgramUniformMatrix4dv(program, location, count, transpose, value);
}

static void set_uniform_n(GLuint shader_id, GLint loc_id, const void *val, GWR_shader_uniform_data_type_t type, GLsizei n) {
    if (loc_id < 0) {
        SHADER_LOG(GWR_LOG_WARNING, "location id < 0");
        return;
    }
    switch (type) {
        case GWR_SHADER_UNIFORM_INT:
            wrapper_gl_prog_uniform1iv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_INT_VEC2:
            wrapper_gl_prog_uniform2iv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_INT_VEC3:
            wrapper_gl_prog_uniform3iv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_INT_VEC4:
            wrapper_gl_prog_uniform4iv(shader_id, loc_id, n, val);
            break;

        case GWR_SHADER_UNIFORM_UINT:
            wrapper_gl_prog_uniform1uiv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_UINT_VEC2:
            wrapper_gl_prog_uniform2uiv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_UINT_VEC3:
            wrapper_gl_prog_uniform3uiv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_UINT_VEC4:
            wrapper_gl_prog_uniform4uiv(shader_id, loc_id, n, val);
            break;

        case GWR_SHADER_UNIFORM_FLOAT:
            wrapper_gl_prog_uniform1fv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_VEC2:
            wrapper_gl_prog_uniform2fv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_VEC3:
            wrapper_gl_prog_uniform3fv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_VEC4:
            wrapper_gl_prog_uniform4fv(shader_id, loc_id, n, val);
            break;

        case GWR_SHADER_UNIFORM_MAT2:
            wrapper_gl_prog_uniform_mat_2fv(shader_id, loc_id, n, GL_FALSE, val);
            break;
        case GWR_SHADER_UNIFORM_MAT3:
            wrapper_gl_prog_uniform_mat_3fv(shader_id, loc_id, n, GL_FALSE, val);
            break;
        case GWR_SHADER_UNIFORM_MAT4:
            wrapper_gl_prog_uniform_mat_4fv(shader_id, loc_id, n, GL_FALSE, val);
            break;

        case GWR_SHADER_UNIFORM_DOUBLE:
            wrapper_gl_prog_uniform1dv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_DOUBLE_VEC2:
            wrapper_gl_prog_uniform2dv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_DOUBLE_VEC3:
            wrapper_gl_prog_uniform3dv(shader_id, loc_id, n, val);
            break;
        case GWR_SHADER_UNIFORM_DOUBLE_VEC4:
            wrapper_gl_prog_uniform4dv(shader_id, loc_id, n, val);
            break;

        case GWR_SHADER_UNIFORM_DOUBLE_MAT2:
            wrapper_gl_prog_uniform_mat_2dv(shader_id, loc_id, n, GL_FALSE, val);
            break;
        case GWR_SHADER_UNIFORM_DOUBLE_MAT3:
            wrapper_gl_prog_uniform_mat_3dv(shader_id, loc_id, n, GL_FALSE, val);
            break;
        case GWR_SHADER_UNIFORM_DOUBLE_MAT4:
            wrapper_gl_prog_uniform_mat_4dv(shader_id, loc_id, n, GL_FALSE, val);
            break;

        default:
            break;
    }
}

static GLboolean check_compile_errors(GLuint shader_id, const char *type) {
    GLint success = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint len = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
        if (len < 1) {
            len = 1;
        }
        char *log = malloc(len);
        if (!log) {
            SHADER_LOG(GWR_LOG_ERROR, "'%s' compile failed (no mem for log)", type);
            return GL_FALSE;
        }
        glGetShaderInfoLog(shader_id, len, NULL, log);
        SHADER_LOG(GWR_LOG_ERROR, "'%s' compilation failed:\n%s", type, log);
        free(log);
        return GL_FALSE;
    }

    return GL_TRUE;
}

static GLboolean check_link_errors(GLuint program) {
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        if (len < 1) {
            len = 1;
        }
        char *log = malloc(len);
        if (!log) {
            SHADER_LOG(GWR_LOG_ERROR, "link failed (no mem for log)");
            return GL_FALSE;
        }
        glGetProgramInfoLog(program, len, NULL, log);
        SHADER_LOG(GWR_LOG_ERROR, "program linking failed:\n%s", log);
        free(log);
        return GL_FALSE;
    }
    return GL_TRUE;
}

static char *read_from_text_file(const char *path, size_t *out_size) {
    assert(path);

    if (out_size) {
        *out_size = 0;
    }

    FILE *f = fopen(path, "rb");
    if (!f) {
        SHADER_LOG(GWR_LOG_ERROR, "can't open file: '%s'", path);
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    const long len = ftell(f);
    if (len < 0) {
        fclose(f);
        return NULL;
    }
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }

    const size_t n = len;
    char *buf = malloc(n + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    const size_t rd = fread(buf, 1, n, f);
    fclose(f);
    if (rd != n) {
        free(buf);
        return NULL;
    }

    buf[n] = '\0';
    if (out_size) {
        *out_size = n;
    }
    return buf;
}
