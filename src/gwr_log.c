#include "gwr_log.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static const char *level_names[] = {
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
};

void GWR_log(GWR_log_level_e level, const char *msg, ...) {
    assert(level < GWR_LOG__COUNT);
    assert(msg);

    printf("[%s] ", level_names[level]);

    FILE *out = level == GWR_LOG_INFO || level == GWR_LOG_WARNING ? stdout : stderr;

    va_list args;
    va_start(args, msg);
    vfprintf(out, msg, args);
    va_end(args);

    fprintf(out, "\n");
    fflush(out);

    if (level == GWR_LOG_FATAL) {
        exit(EXIT_FAILURE);
    }
}
