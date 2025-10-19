#include "game.h"

int main() {
    int error_code = 0;

    // Open a window
    window = mfb_open_ex("The Wonderous Deathly Valley", next_framebuffer_size_x, next_framebuffer_size_y, WF_RESIZABLE);

    mfb_set_target_fps(60);

    // Initialize callbacks
    mfb_set_resize_callback(window, update_size);

    // Setup controls
    const uint8_t *mouse = mfb_get_mouse_button_buffer(window);
    const uint8_t *keys  = mfb_get_key_buffer(window);

    // Initialize Chunk Pool
    new_chunk_array_size = 16;
    error_code = resize_chunk_array();

    for (uint32_t i = 0; i < REGION_ELEMENT_COUNT; i++) {
        region_header_template[i] = NULL_CHUNK;
    }

    if (error_code < 0) {
        printf("Failed to allocate memory for chunk buffers.\n");
        return -1;
    }

    // Main loop:
    do {

        // Ensure the window is of the correct size.
        error_code = resize_window();
        if (error_code < 0) {
            printf("Failed to resize the window.\n");
            return -1;
        }

        // Load chunks.
        load_nearby_chunks();

        // Draw all loaded chunks.
        for (uint32_t i = 0; i < chunk_array_size; i++) {
            draw_chunk(chunk_position_x[i], chunk_position_y[i]);
        }
        
        // Basic movement controls
        camera_position_x -= 4*keys[KB_KEY_A];
        camera_position_x += 4*keys[KB_KEY_D];
        camera_position_y -= 4*keys[KB_KEY_W];
        camera_position_y += 4*keys[KB_KEY_S];

        //printf("Chunk viewport (%d, %d), (%d, %d).\n", viewport_start_chunk_x, viewport_start_chunk_y, viewport_end_chunk_x, viewport_end_chunk_y);

        // Basic world modification logic
        if (mouse[MOUSE_LEFT]) {
            int32_t mouse_x = mfb_get_mouse_x(window);
            int32_t mouse_y = mfb_get_mouse_y(window);

            int32_t mouse_tile_x = FLOOR_DIV(((mouse_x * (int32_t)framebuffer_size_x) / (int32_t)actual_window_size_x + camera_position_x), TILE_SIZE);
            int32_t mouse_tile_y = FLOOR_DIV(((mouse_y * (int32_t)framebuffer_size_y) / (int32_t)actual_window_size_y + camera_position_y), TILE_SIZE);

            set_tile(mouse_tile_x, mouse_tile_y, 1);
        }

        // Update the screen.
        mfb_update_ex(window, framebuffer, framebuffer_size_x, framebuffer_size_y);
    } while (mfb_wait_sync(window));

    // Free all allocated buffers.
    free(framebuffer);
    free(chunk_array);
    free(chunk_flags);
    free(chunk_position_x);
    free(chunk_position_y);
    free(spatial_access_grid);

    return 0;
}