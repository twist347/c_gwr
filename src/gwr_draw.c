#include "gwr_draw.h"

#include <assert.h>

void GWR_draw_arrays(GLenum mode, const GWR_vertex_array_t *vao, const GWR_shader_t *shader, GLint first, GLsizei count) {
    assert(vao);
    assert(shader);

    GWR_shader_use(shader);
    GWR_vertex_array_bind(vao);
    glDrawArrays(mode, first, count);
    GWR_vertex_array_unbind();
}