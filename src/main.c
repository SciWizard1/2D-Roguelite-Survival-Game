#include "game.h"




int main() {
    int error_code = 0;

    // Open a window
    window = mfb_open_ex("The Wonderous Deathly Valley", next_framebuffer_size_x, next_framebuffer_size_y, WF_RESIZABLE);

    // Initialize callbacks
    mfb_set_resize_callback(window, update_size);

    // Setup controls
    const uint8_t *mouse = mfb_get_mouse_button_buffer(window);
    const uint8_t *keys  = mfb_get_key_buffer(window);


    // Initialize Chunk Pool
    new_chunk_array_size = 16;
    error_code = resize_chunk_array();

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

        load_nearby_chunks();
        //camera_position_x += 1;

        // Draw all loaded chunks.

        for (uint32_t i = 0; i < chunk_array_size; i++) {
            //if (chunk_flags[i] == FULL) {
            draw_chunk(chunk_position_x[i], chunk_position_y[i]);
            //}
        }
        
        camera_position_x -= keys[KB_KEY_A];
        camera_position_x += keys[KB_KEY_D];
        camera_position_y -= keys[KB_KEY_W];
        camera_position_y += keys[KB_KEY_S];

        if (mouse[MOUSE_LEFT]) {
            int32_t mouse_x = mfb_get_mouse_x(window);
            int32_t mouse_y = mfb_get_mouse_y(window);

            int32_t mouse_tile_x = ((mouse_x * framebuffer_size_x) / actual_window_size_x + camera_position_x) / TILE_SIZE;
            int32_t mouse_tile_y = ((mouse_y * framebuffer_size_y) / actual_window_size_y + camera_position_y) / TILE_SIZE;

            set_tile(mouse_tile_x, mouse_tile_y, camera_position_z, 1);
        }

        // Update the screen with the framebuffer.
        mfb_update_ex(window, framebuffer, framebuffer_size_x, framebuffer_size_y);


    } while (mfb_wait_sync(window));

    return 0;
}