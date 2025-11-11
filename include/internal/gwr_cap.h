#pragma once

#include <stdbool.h>

typedef enum {
	GWR_FEATURE_DEBUG_OUTPUT = 0,
	GWR_FEATURE_DIRECT_STATE_ACCESS,
	GWR_FEATURE_BUFFER_STORAGE,

	GWR_FEATURE__COUNT
} GWR_feature_e;

bool GWR_cap_init(void);

bool GWR_cap_is_init(void);

void GWR_cap_get_version(int *major, int *minor);

bool GWR_cap_has(GWR_feature_e feature);

