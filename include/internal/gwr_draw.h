#pragma once

#include "gwr_shader.h"
#include "gwr_vertex_array.h"
#include "gwr_element_buffer.h"

void GWR_draw_arrays(
    GLenum mode,
    const GWR_vertex_array_t *vao,
    const GWR_shader_t *shader,
    GLint first,
    GLsizei count
);

void GWR_draw_elements(
    GLenum mode,
    const GWR_vertex_array_t *vao,
    const GWR_shader_t *shader,
    const GWR_element_buffer_t *ebo,
    GLsizei count,
    GLintptr offset
);