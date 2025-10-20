#include "game.h"

// ----------------------------------
// This is where the actual tile data is stored.
uint16_t *chunk_array = NULL;
uint8_t *chunk_flags = NULL;
int32_t *chunk_position_x = NULL;
int32_t *chunk_position_y = NULL;

uint32_t chunk_array_size = 0; // In chunks!
uint32_t new_chunk_array_size = 0;

int resize_chunk_array() {
    // Check if this function was called unnecessarily.
    if (chunk_array_size == new_chunk_array_size) {
        return 0;
    }

    uint8_t null_flag = chunk_flags == NULL;

    // Allocate memory for new buffers.
    chunk_array = tracked_realloc(chunk_array, new_chunk_array_size * CHUNK_SIZE * CHUNK_SIZE * sizeof(uint16_t));
    chunk_flags = tracked_realloc(chunk_flags, new_chunk_array_size * sizeof(uint8_t));
    chunk_position_x = tracked_realloc(chunk_position_x, new_chunk_array_size * sizeof(uint32_t));
    chunk_position_y = tracked_realloc(chunk_position_y, new_chunk_array_size * sizeof(uint32_t));

    // Reset flags to prevent garbage values.
    if (null_flag) {
        for (uint32_t i = 0; i < chunk_array_size; i++) {
            chunk_flags[i] = FREE;
        }
    }

    // Update old values.
    chunk_array_size = new_chunk_array_size;

    return 0;
}

int generate_chunk(int32_t x, int32_t y) {
    // Search the static array for an open slot.
    int32_t chunk_array_index = -1;
    for (int32_t i = 0; i < (int32_t)chunk_array_size; i++) {
        if (chunk_flags[i] == FREE) {
            chunk_array_index = i;
            break;
        }
    }

    // Ensure that the static array has an open spot to generate a new chunk in.
    if (chunk_array_index == -1) {
        return -1;
    }

    // Convert the index into a pointer going directly to the chunk object.
    uint16_t *chunk = &chunk_array[chunk_array_index * CHUNK_SIZE * CHUNK_SIZE];

    // Generate the chunk.
    if (x == 1 && y == 1) {
        for (uint16_t ty = 0; ty < CHUNK_SIZE; ty++) {
            for (uint16_t tx = 0; tx < CHUNK_SIZE; tx++) {
                chunk[ty * CHUNK_SIZE + tx] = ((tx - 128) * (tx - 128) + (ty - 128) * (ty - 128)) < 64 * 64;
            }
        }
    } else {
        for (uint16_t ty = 0; ty < CHUNK_SIZE; ty++) {
            for (uint16_t tx = 0; tx < CHUNK_SIZE; tx++) {
                chunk[ty * CHUNK_SIZE + tx] = ((tx == 0) || (ty == 0));
            }
        }
    }

    // Resize the grid to accommodate the new chunk.
    new_grid_x = MIN(grid_x, x);
    new_grid_y = MIN(grid_y, y);

    new_grid_w = MAX(grid_x + grid_w - 1, x) - new_grid_x + 1;
    new_grid_l = MAX(grid_y + grid_l - 1, y) - new_grid_y + 1;
    
    resize_spatial_access_grid();

    set_chunk(x, y, chunk_array_index);
    chunk_position_x[chunk_array_index] = x;
    chunk_position_y[chunk_array_index] = y;

    chunk_flags[chunk_array_index] = FULL;

    return 0;
}

int load_nearby_chunks() {
    // Unload distant chunks.
    for (uint32_t i = 0; i < chunk_array_size; i++) {
        if (chunk_flags[i] == FULL) {
            // Check if the chunk exists within the viewport bounding box.
            if (
                viewport_start_chunk_x > chunk_position_x[i] || viewport_end_chunk_x < chunk_position_x[i] ||
                viewport_start_chunk_y > chunk_position_y[i] || viewport_end_chunk_y < chunk_position_y[i]
            ) {
                // Temporary logic to delete a chunk without saving.
                chunk_flags[i] = FREE;
                set_chunk(chunk_position_x[i], chunk_position_y[i], NULL_CHUNK);
                printf("Chunk on slot %d has been freed.\n", i);
            }
        }
    }

    // Generate chunks. TODO: Implement chunk loading from the disk.
    for (int32_t y = viewport_start_chunk_y; y < viewport_end_chunk_y; y++) {
        for (int32_t x = viewport_start_chunk_x; x < viewport_end_chunk_x; x++) {
            // Ensure chunks aren't simply regenerated every frame.
            uint32_t chunk_index = get_chunk(x, y);
            if (chunk_index != NULL_CHUNK) {
                continue;
            }
            
            // Generate chunks.
            int error_code = generate_chunk(x, y);
            if (error_code < 0) {
                printf("Failed to load chunk (%d, %d)!\n", x, y);
            } else {
                printf("Successfully loaded chunk (%d, %d)!\n", x, y);
            }
        }
    }

    return 0;
}

// ----------------------------------
// This "spatial access grid" is a data structure used within this program that utilizes what is effectively a 3D lookup table to access a chunk
// in world space. By accessing this grid, you recieve a 32-bit index to the chunk array that can be converted directly into a pointer to that chunk.
uint32_t *spatial_access_grid = NULL;

int32_t grid_x = 0, grid_y = 0; // These coords refer to the northern-western-top corner of the box in terms of world coordinates scaled to chunk units.
int32_t grid_w = 0, grid_l = 0; // In chunks!

// These variables are for updating the variables above.
int32_t new_grid_x = 0, new_grid_y = 0;
int32_t new_grid_w = 0, new_grid_l = 0;

int resize_spatial_access_grid() {
    // Check if this function was called unnecessarily.
    if (grid_x == new_grid_x && grid_y == new_grid_y && grid_w == new_grid_w && grid_l == new_grid_l) {
        return 0;
    }

    // Exit if the old buffer cannot be contained by the new buffer. If this condition is true, that means that data needs to be truncated (NOT IMPLEMENTED!)
    if (grid_x < new_grid_x || grid_y < new_grid_y || grid_x + grid_w > new_grid_x + new_grid_w || grid_y + grid_l > new_grid_y + new_grid_l) {
        return -2;
    }

    // Allocate new memory for the new buffer.
    uint32_t *new_spatial_access_grid = tracked_malloc(new_grid_w * new_grid_l * sizeof(uint32_t));

    // Ensure the allocated memory is rid of garbage values.
    for (int32_t y = 0; y < new_grid_l; y++) {
        for (int32_t x = 0; x < new_grid_w; x++) {
            int32_t new_index = 
                x + 
                y * new_grid_w;
            new_spatial_access_grid[new_index] = NULL_CHUNK;
        }
    }

    // Transfer the data to the new buffer.
    for (int32_t y = 0; y < grid_l; y++) {
        for (int32_t x = 0; x < grid_w; x++) {

            int32_t new_index = 
                (grid_x - new_grid_x + x) + 
                (grid_y - new_grid_y + y) * new_grid_w;

            int32_t old_index = 
                x + 
                y * grid_w;

            new_spatial_access_grid[new_index] = spatial_access_grid[old_index];
        }
    }

    tracked_free(spatial_access_grid);

    // Transfer extra data describing the new buffer.
    spatial_access_grid = new_spatial_access_grid;

    grid_x = new_grid_x; grid_y = new_grid_y;
    grid_w = new_grid_w; grid_l = new_grid_l;

    return 0;
}

uint32_t get_chunk(int32_t x, int32_t y) {
    if (x < grid_x || x >= grid_x + grid_w || y < grid_y || y >= grid_y + grid_l) {
        return NULL_CHUNK;
    }

    int32_t index = spatial_access_grid[(x - grid_x) + (y - grid_y) * grid_w];

    // Return an index to the chunk object.
    return index;
}

void set_chunk(int32_t x, int32_t y, uint32_t index) {
    //if (x < grid_x || x >= grid_x + grid_h || y < grid_y || y >= grid_y + grid_l) {
    //    printf("Failed to set chunk (%d, %d, %d)!\n", x, y, z);
    //    return;
    //}
    int32_t flat_access_index = (x - grid_x) + (y - grid_y) * grid_w;
    
    spatial_access_grid[flat_access_index] = index;
}

int set_tile(int32_t x, int32_t y, uint16_t tile_id) {
    int32_t cx = FLOOR_DIV(x, CHUNK_SIZE);
    int32_t cy = FLOOR_DIV(y, CHUNK_SIZE);

    uint32_t chunk_index = get_chunk(cx, cy);
    

    if (chunk_index == NULL_CHUNK) {
        // Chunk is not loaded.
        return -1;
    }

    chunk_array[
        chunk_index * CHUNK_SIZE * CHUNK_SIZE + 
        (x & CHUNK_MASK) +
        (y & CHUNK_MASK) * CHUNK_SIZE
    ] = tile_id;

    return 0;
}