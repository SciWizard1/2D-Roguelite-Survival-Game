#include "game.h"


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Fullscreen Triangles", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Init GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }
    
    new_chunk_array_size = 16;

    // General-use error value for the main loop.
    //int error_code = 0;

    // Open a window
    //window = mfb_open_ex("The Wonderous Deathly Valley", next_framebuffer_size_x, next_framebuffer_size_y, WF_RESIZABLE);

    //mfb_set_target_fps(60);
    glfwSwapInterval(1);

    // Initialize callbacks
    //mfb_set_resize_callback(window, update_window_size);

    // Setup controls
    //const uint8_t *mouse = mfb_get_mouse_button_buffer(window);
    //const uint8_t *keys  = mfb_get_key_buffer(window);

    glfwSetKeyCallback(window, keyboard_event_callback);
    glfwSetMouseButtonCallback(window, mouse_button_event_callback);
    glfwSetCursorPosCallback(window, cursor_position_event_callback);



    // Initialize memory regions.
    initialize_blank_region_header();
    initialize_tracked_memory_buffers();

    // Initialize Chunk Pool
    resize_chunk_array();

    new_entity_pool_size = 16;
    resize_entity_array();

    TILE_SIZE_EXPONENT = 4;
    update_tile_size();

    //fps_timer = mfb_timer_create();

    entity_pos_x[player_index] = 0;
    entity_pos_y[player_index] = 0;

    // Main loop:
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Ensure the window is of the correct size.
        //error_code = resize_window();
        //if (error_code < 0) {
        //    printf("Failed to resize the window.\n");
        //    return -1;
        //}

        // Load chunks.
        // load_nearby_chunks();

        // Draw all loaded chunks.
        //for (uint32_t i = 0; i < chunk_array_size; i++) {
        //    draw_chunk(chunk_position_x[i], chunk_position_y[i]);
        //}
        
        // Basic movement controls
        entity_pos_x[player_index] -= 8 * keyboard[GLFW_KEY_A];
        entity_pos_x[player_index] += 8 * keyboard[GLFW_KEY_D];
        entity_pos_y[player_index] -= 8 * keyboard[GLFW_KEY_W];
        entity_pos_y[player_index] += 8 * keyboard[GLFW_KEY_S];

        // Move the camera towards the player (0b1110 is the best approximation of 0.9 in Q28.4)
        camera_position_x = camera_position_x + (int32_t)((((int64_t)(entity_pos_x[player_index] - camera_position_x)) * 1) >> 4);
        camera_position_y = camera_position_y + (int32_t)((((int64_t)(entity_pos_y[player_index] - camera_position_y)) * 1) >> 4);
        
        //printf("Chunk viewport (%d, %d), (%d, %d).\n", viewport_start_chunk_x, viewport_start_chunk_y, viewport_end_chunk_x, viewport_end_chunk_y);

        // Basic world modification logic
        //if (mouse[MOUSE_LEFT]) {
        //    int32_t mouse_x = mfb_get_mouse_x(window);
        //    int32_t mouse_y = mfb_get_mouse_y(window);

        //    int32_t mouse_tile_x = FLOOR_DIV(((mouse_x * (int32_t)framebuffer_size_x) / (int32_t)actual_window_size_x + camera_position_x), (int32_t)TILE_SIZE);
        //    int32_t mouse_tile_y = FLOOR_DIV(((mouse_y * (int32_t)framebuffer_size_y) / (int32_t)actual_window_size_y + camera_position_y), (int32_t)TILE_SIZE);
        //    set_tile(mouse_tile_x, mouse_tile_y, 1);
        //}

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update the screen.
        //mfb_update_ex(window, framebuffer, framebuffer_size_x, framebuffer_size_y);

        //double frame_time = mfb_timer_delta(fps_timer);
        //if (frame_time > 0.03f) {
        //    printf("MS THIS FRAME: %lf\n", frame_time * 1000.0f);
        //}

        glfwSwapBuffers(window);
    };

    // Free all allocated buffers.
    tracked_free(framebuffer);
    tracked_free(chunk_array);
    tracked_free(chunk_flags);
    tracked_free(chunk_position_x);
    tracked_free(chunk_position_y);
    tracked_free(tile_spatial_access_grid);
    tracked_free(nbt_spatial_access_grid);
    tracked_free(entity_ids);
    tracked_free(entity_pos_x);
    tracked_free(entity_pos_y);

    glfwDestroyWindow(window);
    glfwTerminate();

    printf("Exited with %d unfreed memory blocks totaling %d bytes!\n", pointer_stack_top, stack_memory_usage);

    for (uint32_t i = 0; i < pointer_stack_top; i++) {
        printf("Unfreed pointer %lX with %d bytes.\n", (uint64_t)pointer_stack[i], buffer_sizes[i]);
    }

    return 0;
}