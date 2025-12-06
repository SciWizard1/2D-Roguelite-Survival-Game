#include "game.h"

GLuint shader_program;

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
    shader_program = link_shaders();
    if (!shader_program) {
        printf("Failed to create shader program.\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //glfwSwapInterval(1);

    glUseProgram(shader_program);

    // Setup controls
    glfwSetKeyCallback(window, keyboard_event_callback);
    glfwSetMouseButtonCallback(window, mouse_button_event_callback);
    glfwSetCursorPosCallback(window, cursor_position_event_callback);

    // Get uniform positions.
    int uniform_camera_position_x = glGetUniformLocation(shader_program, "camera_position_x");
    int uniform_camera_position_y = glGetUniformLocation(shader_program, "camera_position_y");
    int uniform_camera_zoom       = glGetUniformLocation(shader_program, "camera_zoom");
    int uniform_aspect_ratio      = glGetUniformLocation(shader_program, "aspect_ratio");
    int uniform_chunk_array_size  = glGetUniformLocation(shader_program, "chunk_array_size");

    int uniform_spatial_access_grid_x = glGetUniformLocation(shader_program, "spatial_access_grid_x");
    int uniform_spatial_access_grid_y = glGetUniformLocation(shader_program, "spatial_access_grid_y");
    int uniform_spatial_access_grid_w = glGetUniformLocation(shader_program, "spatial_access_grid_w");
    int uniform_spatial_access_grid_l = glGetUniformLocation(shader_program, "spatial_access_grid_l");

    // Initialize memory regions.
    initialize_blank_region_header();
    initialize_tracked_memory_buffers();

    // Initialize Chunk Pool
    resize_chunk_array();
    resize_entity_array();

    // Update tile size with default value.
    update_tile_size();

    // Initialize player position.
    entity_pos_x[player_index] = 0;
    entity_pos_y[player_index] = 0;

    // Initialize GPU buffers for transferring larger data.
    // Initialize tile data buffers.

    texture_instance chunk_array_texture_uniform = generate_storage_texture_array("chunk_array", CHUNK_SIZE, CHUNK_SIZE, chunk_array_size);
    texture_instance tile_texture_array_uniform  = generate_graphics_texture_array("tile_textures", TILE_SIZE, TILE_SIZE, num_textures);

    // Initialize chunk lookup buffers.
    GLuint gpu_spatial_access_grid_uniform_buffer;
    glGenBuffers(1, &gpu_spatial_access_grid_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, gpu_spatial_access_grid_uniform_buffer);

    GLsizeiptr gpu_spatial_access_grid_max_size = 1024;
    glBufferData(GL_UNIFORM_BUFFER, gpu_spatial_access_grid_max_size, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, gpu_spatial_access_grid_uniform_buffer);

    for (uint32_t i = 0; i < num_textures; i++) {
        glActiveTexture(GL_TEXTURE0 + tile_texture_array_uniform.binding_index);
        glBindTexture(GL_TEXTURE_2D_ARRAY, tile_texture_array_uniform.descriptor);

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0, i,
            TILE_SIZE,
            TILE_SIZE,
            1,
            GL_RGBA,
            GL_UNSIGNED_INT_8_8_8_8_REV,
            &textures[i * TILE_SIZE * TILE_SIZE]
        );
    }


    double TARGET_FPS = 240.0;
    double FRAME_TIME_LIMIT = 1.0 / TARGET_FPS;
    double last_frame_time = 0.0;

    // Main loop:
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetWindowSize(window, &window_width, &window_height);
        aspect_ratio = (float)window_width / (float)window_height;

        // FPS calculations
        double current_time = glfwGetTime();
        double delta_time = current_time - last_frame_time;
        
        //camera_position_x = entity_pos_x[player_index];
        //camera_position_y = entity_pos_y[player_index];

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

        update_viewport();

        // Load chunks.
        load_nearby_chunks();


        if (delta_time >= FRAME_TIME_LIMIT) {

            // Basic movement controls
            entity_pos_x[player_index] -= 0.5 * keyboard[GLFW_KEY_A];
            entity_pos_x[player_index] += 0.5 * keyboard[GLFW_KEY_D];
            entity_pos_y[player_index] += 0.5 * keyboard[GLFW_KEY_W];
            entity_pos_y[player_index] -= 0.5 * keyboard[GLFW_KEY_S];

            // Move the camera towards the player.
            camera_position_x = camera_position_x + (entity_pos_x[player_index] - camera_position_x) * 0.1;
            camera_position_y = camera_position_y + (entity_pos_y[player_index] - camera_position_y) * 0.1;

            update_gpu_spatial_access_grid();

            // Set uniforms
            glUniform1f(uniform_camera_position_x, camera_position_x);
            glUniform1f(uniform_camera_position_y, camera_position_y);
            glUniform1f(uniform_camera_zoom,       camera_zoom);
            glUniform1f(uniform_aspect_ratio,      aspect_ratio);

            glUniform1ui(uniform_chunk_array_size,  chunk_array_size);

            glUniform1i(uniform_spatial_access_grid_x, gpu_grid_x);
            glUniform1i(uniform_spatial_access_grid_y, gpu_grid_y);
            glUniform1i(uniform_spatial_access_grid_w, gpu_grid_w);
            glUniform1i(uniform_spatial_access_grid_l, gpu_grid_l);

            for (uint32_t i = 0; i < chunk_array_size; i++) {
                update_chunk_texture(chunk_array_texture_uniform, i);
            }

            // Upload chunk lookup table

            int32_t size_of_gpu_spatial_access_grid = gpu_grid_w * gpu_grid_l * sizeof(chunk_entry_padded);
            glBindBuffer(GL_UNIFORM_BUFFER, gpu_spatial_access_grid_uniform_buffer);

            void* ptr = glMapBufferRange(
                GL_UNIFORM_BUFFER,
                0,
                size_of_gpu_spatial_access_grid,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
            );

            memcpy(ptr, gpu_spatial_access_grid, size_of_gpu_spatial_access_grid);
            glUnmapBuffer(GL_UNIFORM_BUFFER);

            // Run shader.
            glUseProgram(shader_program);
            glBindVertexArray(vertex_array_object);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            //glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glfwSwapBuffers(window);
            last_frame_time = current_time;

            printf("FPS: %lf\n", 1/delta_time);
        }
    };

    // Free all allocated buffers.
    tracked_free(framebuffer);
    tracked_free(chunk_array);
    tracked_free(chunk_flags);
    tracked_free(chunk_position_x);
    tracked_free(chunk_position_y);
    tracked_free(spatial_access_grid);
    tracked_free(entity_ids);
    tracked_free(entity_pos_x);
    tracked_free(entity_pos_y);
    tracked_free(gpu_spatial_access_grid);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

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