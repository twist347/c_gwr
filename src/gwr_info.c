#include "internal/gwr_info.h"

#include <string.h>
#include <stdio.h>

#include "glad/glad.h"

void GWR_info_print(void) {
    GLint major = 0, minor = 0, profile = 0, n_ext = 0;
    const char *vendor = (const char *) glGetString(GL_VENDOR);
    const char *renderer = (const char *) glGetString(GL_RENDERER);
    const char *version = (const char *) glGetString(GL_VERSION);
    const char *glsl = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    glGetIntegerv(GL_NUM_EXTENSIONS, &n_ext);

    const char *profile_str =
            profile & GL_CONTEXT_CORE_PROFILE_BIT
                ? "Core"
                : profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
                      ? "Compatibility"
                      : "Unknown";

    fprintf(stdout, "---------------- OpenGL context ----------------\n");
    fprintf(stdout, "Version:    %d.%d (%s)\n", major, minor, version ? version : "?");
    fprintf(stdout, "GLSL:       %s\n", glsl ? glsl : "?");
    fprintf(stdout, "Vendor:     %s\n", vendor ? vendor : "?");
    fprintf(stdout, "Renderer:   %s\n", renderer ? renderer : "?");
    fprintf(stdout, "Profile:    %s\n", profile_str);
    fprintf(stdout, "Extensions: %d\n", n_ext);
    fprintf(stdout, "------------------------------------------------\n");
}
