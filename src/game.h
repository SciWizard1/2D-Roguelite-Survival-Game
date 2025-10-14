#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "MiniFB.h"

// Special value indicating no chunk is assigned to this cell.
// Must NOT be 0, since 0 is a valid chunk index!
#define NULL_CHUNK 0xFFFFFFFF
#define CHUNK_SIZE 256
#define CHUNK_MASK (CHUNK_SIZE - 1)
#define TILE_SIZE 16
#define TILE_MASK (TILE_SIZE - 1)

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

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

int resize_chunk_array();
int generate_chunk(int32_t x, int32_t y, int32_t z);

extern uint16_t *chunk_array;
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
