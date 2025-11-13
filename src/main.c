#include "game.h"

int main() {

    if (initialize_window_context() != 0) {
        return -1;
    }

    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    setup_vertex_buffers();

    // Setup vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Setup shaders
    GLuint shader_program = link_shaders();
    if (!shader_program) {
        printf("Failed to create shader program.\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //glfwSwapInterval(1);


    // Setup controls
    glfwSetKeyCallback(window, keyboard_event_callback);
    glfwSetMouseButtonCallback(window, mouse_button_event_callback);
    glfwSetCursorPosCallback(window, cursor_position_event_callback);

    // Get uniform positions.
    int uniform_camera_position_x = glGetUniformLocation(shader_program, "camera_position_x");
    int uniform_camera_position_y = glGetUniformLocation(shader_program, "camera_position_y");
    int uniform_camera_zoom       = glGetUniformLocation(shader_program, "camera_zoom");
    int uniform_aspect_ratio      = glGetUniformLocation(shader_program, "aspect_ratio");

    // Initialize memory regions.
    initialize_blank_region_header();
    initialize_tracked_memory_buffers();

    // Initialize Chunk Pool
    resize_chunk_array();
    resize_entity_array();

    update_tile_size();

    entity_pos_x[player_index] = 0;
    entity_pos_y[player_index] = 0;


    // Initialize GPU buffers for transferring larger data.

    GLuint chunk_array_shader_storage_buffer_object;
    glGenBuffers(1, &chunk_array_shader_storage_buffer_object);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunk_array_shader_storage_buffer_object);

    GLsizeiptr chunk_array_shader_buffer_size = chunk_array_size * CHUNK_TEXTURE_BYTES;

    glBufferStorage(GL_SHADER_STORAGE_BUFFER, chunk_array_shader_buffer_size, NULL, 
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
    );

    void* chunk_array_shader_buffer_pointer = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, chunk_array_shader_buffer_size,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
    );


    // Main loop:
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetWindowSize(window, &window_width, &window_height);
        aspect_ratio = (float)window_width / (float)window_height;

        // Ensure the window is of the correct size.
        //error_code = resize_window();
        //if (error_code < 0) {
        //    printf("Failed to resize the window.\n");
        //    return -1;
        //}

        // Load chunks.
        load_nearby_chunks();
        
        // Basic movement controls
        entity_pos_x[player_index] -= 0.1 * keyboard[GLFW_KEY_A];
        entity_pos_x[player_index] += 0.1 * keyboard[GLFW_KEY_D];
        entity_pos_y[player_index] += 0.1 * keyboard[GLFW_KEY_W];
        entity_pos_y[player_index] -= 0.1 * keyboard[GLFW_KEY_S];

        printf("Player is at (%f, %f)\r", entity_pos_x[player_index], entity_pos_y[player_index]);

        // Move the camera towards the player.
        camera_position_x = camera_position_x + (entity_pos_x[player_index] - camera_position_x) * 0.1;
        camera_position_y = camera_position_y + (entity_pos_y[player_index] - camera_position_y) * 0.1;
        
        //printf("Chunk viewport (%d, %d), (%d, %d).\n", viewport_start_chunk_x, viewport_start_chunk_y, viewport_end_chunk_x, viewport_end_chunk_y);

        // Basic world modification logic
        //if (mouse[MOUSE_LEFT]) {
        //    int32_t mouse_x = mfb_get_mouse_x(window);
        //    int32_t mouse_y = mfb_get_mouse_y(window);

        //    int32_t mouse_tile_x = FLOOR_DIV(((mouse_x * (int32_t)framebuffer_size_x) / (int32_t)actual_window_size_x + camera_position_x), (int32_t)TILE_SIZE);
        //    int32_t mouse_tile_y = FLOOR_DIV(((mouse_y * (int32_t)framebuffer_size_y) / (int32_t)actual_window_size_y + camera_position_y), (int32_t)TILE_SIZE);
        //    set_tile(mouse_tile_x, mouse_tile_y, 1);
        //}

        
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        // Copy data to large GPU buffers.

        memcpy(
            (uint8_t*)chunk_array_shader_buffer_pointer, 
            chunk_array, CHUNK_TEXTURE_BYTES
        );

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 
            chunk_array_shader_storage_buffer_object, 
            0, 
            chunk_array_shader_buffer_size
        );

        // Set uniforms
        glUniform1f(uniform_camera_position_x, camera_position_x);
        glUniform1f(uniform_camera_position_y, camera_position_y);
        glUniform1f(uniform_camera_zoom,       camera_zoom);
        glUniform1f(uniform_aspect_ratio,      aspect_ratio);

        // Run shader.
        glUseProgram(shader_program);
        glBindVertexArray(vertex_array_object);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        //glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glDeleteBuffers(1, &chunk_array_shader_storage_buffer_object);

    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteBuffers(1, &vertex_buffer_object);
    glfwDestroyWindow(window);
    glfwTerminate();

    printf("Exited with %d unfreed memory blocks totaling %d bytes!\n", pointer_stack_top, stack_memory_usage);

    for (uint32_t i = 0; i < pointer_stack_top; i++) {
        printf("Unfreed pointer %lX with %d bytes.\n", (uint64_t)pointer_stack[i], buffer_sizes[i]);
    }

    return 0;
}