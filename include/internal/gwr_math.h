#pragma once

// constants

#define GWR_PI 3.14159265358979323846f
#define GWR_DEG2RAD (GLT_PI / 180.f)
#define GWR_RAD2DEG (180.f / GLT_PI)

// types

typedef struct {
    float x;
    float y;
} GWR_vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} GWR_vec3_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} GWR_vec4_t;

typedef struct {
    float pos[3];
    float color[3];
    float tex_coord[2];
} GWR_vert_t;

// funcs

float GWR_clamp(float val, float min_val, float max_val);

float GWR_lerp(float a, float b, float t);
