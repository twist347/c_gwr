#pragma once

#include "gwr_shader.h"
#include "gwr_vertex_array.h"
#include "gwr_element_buffer.h"

void GWR_draw_arrays(GLenum mode, const GWR_vertex_array_t *vao, const GWR_shader_t *shader, GLint first, GLsizei count);
