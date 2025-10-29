#pragma once

#include "glad/glad.h"

// TODO: unpack color macro

#define GWR_UNPACK_COLOR(color) ((color)[0]), ((color)[1]), ((color)[2])

#define GWR_RED   ((GLfloat[3]){1.f, 0.f, 0.f})
#define GWR_GREEN ((GLfloat[3]){0.f, 1.f, 0.f})
#define GWR_BLUE  ((GLfloat[3]){0.f, 0.f, 1.f})
#define GWR_BLACK ((GLfloat[3]){0.f, 0.f, 0.f})
#define GWR_WHITE ((GLfloat[3]){1.f, 1.f, 1.f})