#include "internal/gwr_math.h"

float GWR_clamp(float val, float min_val, float max_val) {
    if (val < min_val) {
        return min_val;
    }
    if (val > max_val) {
        return max_val;
    }
    return val;
}

float GWR_lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
