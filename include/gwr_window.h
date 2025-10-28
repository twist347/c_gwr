#pragma once

#include <stdbool.h>

typedef struct GWR_window_t GWR_window_t;

GWR_window_t *GWR_window_create(int width, int height, const char *title);

void GWR_window_destroy(GWR_window_t *window);

// returns GLFWwindow *
void *GWR_window_get_handle(const GWR_window_t *window);

void GWR_window_set_vsync(bool enabled);

bool GWR_window_should_close(const GWR_window_t *window);

void GWR_window_process_input(const GWR_window_t *window);

void GWR_window_swap_buffers(const GWR_window_t *window);

void GWR_window_poll_events(void);

void GWR_window_set_clear_color(float r, float g, float b, float a);

void GWR_window_clear(void);

void GWR_window_set_current(GWR_window_t *window);

int GWR_window_get_width(const GWR_window_t *window);

int GWR_window_get_height(const GWR_window_t *window);
