#pragma once

typedef enum {
    GWR_LOG_INFO = 0,
    GWR_LOG_WARNING,
    GWR_LOG_ERROR,
    GWR_LOG_FATAL,

    GWR_LOG__COUNT
} GWR_log_level_e;

void GWR_log(GWR_log_level_e level, const char *msg, ...);