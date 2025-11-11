#pragma once

#include <stdio.h>
#include <stdlib.h>

#define GWR_ARR_LEN(arr)    (sizeof((arr)) / sizeof((arr)[0]))

#define GWR_UNUSED(val)     ((void) (val))

#define GWR_ASSERT_MSG(cond, msg)                                            \
    do {                                                                     \
        if (!(cond)) {                                                       \
            fprintf(                                                         \
                stderr,                                                      \
                "[GWR]: assertion failed: %s. file: %s, line: %d. %s\n",     \
                #cond, __FILE__, __LINE__, (msg)                             \
            );                                                               \
            abort();                                                         \
        }                                                                    \
    } while (0)

#define GWR_ASSERT(cond)    GWR_ASSERT_MSG(cond, "")

#define GWR_UNIMPLEMENTED()                                                  \
    do {                                                                     \
        fprintf(                                                             \
            stderr,                                                          \
            "[GWR]: unimplemented: %s (%s:%d)", __func__, __FILE__, __LINE__ \
        );                                                                   \
        abort();                                                             \
    } while (0)                                                              \

#define GWR_UNREACHABLE()                                                    \
    do {                                                                     \
        fprintf(stderr, "[GWR]: unreachable");                               \
        abort();                                                             \
    } while (0)                                     
