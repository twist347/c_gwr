#include <math.h>
#include <stdlib.h>

#include "gwr.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE "01_window_example"

#define BG_COLOR GWR_WHITE

int main() {
    int exit_code = EXIT_SUCCESS;
    GWR_window_t *window = NULL;

    window = GWR_window_create(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    GWR_info_print();

    GWR_window_set_clear_color(GWR_UNPACK_COLOR(BG_COLOR), 1.f);

    while (!GWR_window_should_close(window)) {
        GWR_window_process_input(window);

        GWR_window_clear();

        GWR_window_swap_buffers(window);
        GWR_window_poll_events();
    }

cleanup:
    if (window) {
        GWR_window_destroy(window);
    }

    return exit_code;
}
