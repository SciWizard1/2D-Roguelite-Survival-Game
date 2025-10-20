#include "game.h"

// Path to all saves
char *save_directory_path = NULL; // Must end with '/' character

// Name of selected save directory
char *selected_save_directory = NULL; // Must end with '/' character

char tile_data_directory[11] = "tile_data/";
char region_file_name[37]; // "region_x_y_z.bin", assuming x, y, and z max at 8 characters each.
char *region_file_directory = NULL;

uint32_t region_header_template[REGION_ELEMENT_COUNT];

void create_directory(const char *path) {
    #ifdef _WIN32
        _mkdir(path);
    #else
        mkdir(path, 0777);
    #endif
}

int load_chunk_from_disk(int32_t x, int32_t y) {
    (void)x;
    (void)y;
    return 0;
}

int save_chunk_to_disk(int32_t x, int32_t y) {
    (void)x;
    (void)y;
    return 0;
}

void create_new_save(const char *path) {
    create_directory(path);
    char *next_used_path = concatenate_strings(path, &tile_data_directory[0]);
    create_directory(next_used_path);
}

int create_region(int32_t x, int32_t y) {
    sprintf(region_file_name, "region_%d_%d.bin", x, y);

    int32_t region_directory_size = strlen(save_directory_path) + strlen(selected_save_directory) + strlen(tile_data_directory) + strlen(region_file_name) + 1;

    region_file_directory = tracked_malloc(region_directory_size);
    if (region_file_directory == NULL) {
        return -1;
    }

    uint32_t i = 0;
    for (uint32_t j = 0; j < strlen(save_directory_path); j++, i++) {
        region_file_directory[i] = save_directory_path[j];
    }
    for (uint32_t j = 0; j < strlen(selected_save_directory); j++, i++) {
        region_file_directory[i] = selected_save_directory[j];
    }
    for (uint32_t j = 0; j < strlen(tile_data_directory); j++, i++) {
        region_file_directory[i] = tile_data_directory[j];
    } 
    for (uint32_t j = 0; j < strlen(region_file_name); j++, i++) {
        region_file_directory[i] = region_file_name[j];
    }
    region_file_directory[i] = 0;

    printf(region_file_directory);
    printf("\n");

    FILE *region_file;
    region_file = fopen(region_file_directory, "wb");

    if (region_file == NULL) {
        printf("Failed to open file!\n");
        return -1;
    }

    fwrite(region_header_template, sizeof(uint32_t), REGION_ELEMENT_COUNT, region_file);
    fclose(region_file);
    tracked_free(region_file_directory);
    return 0;
}

void initialize_blank_region_header() { 
    for (uint32_t i = 0; i < REGION_ELEMENT_COUNT; i++) {
        region_header_template[i] = NULL_CHUNK;
    }
}