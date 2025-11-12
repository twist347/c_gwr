#pragma once

#include <stdio.h>
#include <stdlib.h>

#define GWR_ARR_LEN(arr)    (sizeof((arr)) / sizeof((arr)[0]))

#define GWR_UNUSED(val)     ((void) (val))

#define GWR_VERIFY_MSG(cond, msg)                                              \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(                                                           \
                stderr,                                                        \
                "[GWR]: verify failed: %s. file: %s, line: %d. %s\n",          \
                #cond, __FILE__, __LINE__, (msg)                               \
            );                                                                 \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define GWR_VERIFY(cond)    GWR_VERIFY_MSG(cond, "")

#define GWR_UNIMPLEMENTED()                                                    \
    do {                                                                       \
        fprintf(                                                               \
            stderr,                                                            \
            "[GWR]: unimplemented: %s (%s:%d)\n",                              \
            __func__, __FILE__, __LINE__                                       \
        );                                                                     \
        fflush(stderr);                                                        \
        abort();                                                               \
    } while (0)                                                                \

#define GWR_UNREACHABLE()                                                      \
    do {                                                                       \
        fprintf(                                                               \
            stderr,                                                            \
             "[GWR]: unreachable: %s (%s:%d)\n",                               \
            __func__, __FILE__, __LINE__                                       \
        );                                                                     \
        fflush(stderr);                                                        \
        abort();                                                               \
    } while (0)                                     

#define GWR_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
