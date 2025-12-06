#include "game.h"

int32_t texture_buffer_bind_index;

texture_instance generate_storage_texture_array(char *shader_uniform_name, int width, int height, int array_size) {
    texture_instance new_texture_instance;

    glGenTextures(1, &new_texture_instance.descriptor);

    glActiveTexture(GL_TEXTURE0 + texture_buffer_bind_index);
    glBindTexture(GL_TEXTURE_2D_ARRAY, new_texture_instance.descriptor);

    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY,
        1,
        GL_R16UI,
        width,
        height,
        array_size
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glUniform1i(glGetUniformLocation(shader_program, shader_uniform_name), texture_buffer_bind_index);

    new_texture_instance.binding_index = texture_buffer_bind_index;

    texture_buffer_bind_index++;
    return new_texture_instance;
}

texture_instance generate_graphics_texture_array(char *shader_uniform_name, int width, int height, int array_size) {
    texture_instance new_texture_instance;
    
    glGenTextures(1, &new_texture_instance.descriptor);

    glActiveTexture(GL_TEXTURE0 + texture_buffer_bind_index);
    glBindTexture(GL_TEXTURE_2D_ARRAY, new_texture_instance.descriptor);

    glPixelStorei(GL_UNPACK_ALIGNMENT, texture_buffer_bind_index);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0, 
        GL_RGBA8, 
        width, 
        height, 
        array_size, 
        0, 
        GL_RGBA, 
        GL_UNSIGNED_INT_8_8_8_8_REV, 
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

    glUniform1i(glGetUniformLocation(shader_program, shader_uniform_name), texture_buffer_bind_index);

    texture_buffer_bind_index++;
    return new_texture_instance;
}