#include "internal/gwr_cap.h"
#include "internal/gwr_log.h"

#include "glad/glad.h"

#include <stddef.h>

#define CAP_LOG(level, msg, ...)    GWR_log((level), "[CAPS]: " msg, ##__VA_ARGS__)

typedef struct {
	int major;
	int minor;

	bool has_buffer_storage;
	bool has_dsa;
	bool has_debug_output;
} GWR_cap_t;

static GWR_cap_t s_cap;
static bool s_inited = false;

static void detect_version(GWR_cap_t *cap);

static void detect_features(GWR_cap_t *cap);

bool GWR_cap_init(void) {
	if (s_inited) {
		return true;
	}

	s_cap.major = 0;
	s_cap.minor = 0;
	s_cap.has_buffer_storage = false;
	s_cap.has_dsa = false;
	s_cap.has_debug_output = false;

	detect_version(&s_cap);
	detect_features(&s_cap);

	s_inited = true;

	return true;
}

bool GWR_cap_is_init(void) {
	return s_inited;
}

void GWR_cap_get_version(int *major, int *minor) {
	if (major) {
		*major = s_inited ? s_cap.major : 0;
	}
	if (minor) {
		*minor = s_inited ? s_cap.minor : 0;
	}
}

bool GWR_cap_has(GWR_feature_e feature) {
	if (!s_inited) {
		return false;
	}

	switch (feature) {
		case GWR_FEATURE_DEBUG_OUTPUT:
			return s_cap.has_debug_output;
		case GWR_FEATURE_BUFFER_STORAGE:
			return s_cap.has_buffer_storage;
		case GWR_FEATURE_DIRECT_STATE_ACCESS:
			return s_cap.has_dsa;
		default:
			return false;
	}
}

static void detect_version(GWR_cap_t *cap) {
	GLint major = 0, minor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	cap->major = major;
	cap->minor = minor;
}

static void detect_features(GWR_cap_t *cap) {
	cap->has_dsa = GLAD_GL_VERSION_4_5 || GLAD_GL_ARB_direct_state_access;
	cap->has_buffer_storage = GLAD_GL_VERSION_4_4 || GLAD_GL_ARB_buffer_storage;
	cap->has_debug_output = GLAD_GL_VERSION_4_3 || GLAD_GL_KHR_debug  || GLAD_GL_ARB_debug_output;
}
