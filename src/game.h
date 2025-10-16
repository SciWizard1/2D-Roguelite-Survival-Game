#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "MiniFB.h"

// Special value indicating no chunk is assigned to this cell.
// Must NOT be 0, since 0 is a valid chunk index!
#define NULL_CHUNK 0xFFFFFFFF

// Chunk and tile information. Tile size should be made into a variable to allow for differently sized textures.
// Masks are for running modulo operators using the AND bitwise operator.
#define CHUNK_SIZE 256
#define CHUNK_MASK (CHUNK_SIZE - 1)
#define TILE_SIZE 16
#define TILE_MASK (TILE_SIZE - 1)

// Utilities
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define FLOOR_DIV(a,b) ((a >= 0) ? (a / b) : ((a - b + 1) / b))
#define CEIL_DIV(x, y) (((x) + (y) - 1) / (y))

// For finding what chunks to load.

#define viewport_start_chunk_x FLOOR_DIV(camera_position_x, (TILE_SIZE * CHUNK_SIZE))
#define viewport_end_chunk_x CEIL_DIV(camera_position_x + (int32_t)framebuffer_size_x, (TILE_SIZE * CHUNK_SIZE))
#define viewport_start_chunk_y FLOOR_DIV(camera_position_y, (TILE_SIZE * CHUNK_SIZE))
#define viewport_end_chunk_y CEIL_DIV(camera_position_y + (int32_t)framebuffer_size_y, (TILE_SIZE * CHUNK_SIZE))
#define viewport_start_chunk_z (camera_position_z)
#define viewport_end_chunk_z (camera_position_z + 1)

int resize_window();
void update_size(struct mfb_window *window, int width, int height);
void draw_chunk(int32_t cx, int32_t cy);

extern struct mfb_window *window;
extern uint32_t *framebuffer;

extern uint32_t framebuffer_size_x;
extern uint32_t framebuffer_size_y;
extern uint32_t next_framebuffer_size_x;
extern uint32_t next_framebuffer_size_y;
extern uint32_t actual_window_size_x;
extern uint32_t actual_window_size_y;

extern int32_t camera_position_x;
extern int32_t camera_position_y;
// Integer format
extern int32_t camera_position_z;

extern uint32_t textures[];

// --------------------

typedef enum {
    FREE = 0,
    FULL = 1,
} loaded_chunk_pool_flags;

int resize_chunk_array();
int generate_chunk(int32_t x, int32_t y, int32_t z);
int load_nearby_chunks();

extern uint16_t *chunk_array;
extern uint8_t *chunk_flags;
extern int32_t *chunk_position_x;
extern int32_t *chunk_position_y;
extern int32_t *chunk_position_z;

extern uint32_t chunk_array_size; // In chunks!
extern uint32_t new_chunk_array_size;

int resize_spatial_access_grid();
uint32_t get_chunk(int32_t x, int32_t y, int32_t z);
void set_chunk(int32_t x, int32_t y, int32_t z, uint32_t index);
int set_tile(int32_t x, int32_t y, int32_t z, uint16_t tile_id);

extern uint32_t *spatial_access_grid;
extern int32_t grid_x, grid_y, grid_z; // These coords refer to the northern-western-top corner of the box in terms of world coordinates scaled to chunk units.
extern int32_t grid_w, grid_l, grid_h; // In chunks!
// These variables are for updating the variables above.
extern int32_t new_grid_x, new_grid_y, new_grid_z;
extern int32_t new_grid_w, new_grid_l, new_grid_h;
