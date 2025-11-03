#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MiniFB.h"

#define KEY_COUNT (GLFW_KEY_LAST + 1)
#define MOUSE_BUTTON_COUNT (GLFW_MOUSE_BUTTON_LAST + 1)

// Special value indicating no chunk is assigned to this cell.
// Must NOT be 0, since 0 is a valid chunk index!
#define NULL_CHUNK 0xFFFFFFFF

// Chunk and tile information.
// Masks are for running modulo operators using the AND bitwise operator.
#define CHUNK_SIZE 256
#define CHUNK_MASK (CHUNK_SIZE - 1)

// Variable names are capitalized due to migration from value definitions.
extern uint32_t TILE_SIZE_EXPONENT; // In powers of two
extern uint32_t TILE_SIZE;
extern uint32_t TILE_MASK;
//#define TILE_SIZE 16
//#define TILE_MASK (TILE_SIZE - 1)

// Utilities
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define FLOOR_DIV(a,b) ((a >= 0) ? (a / b) : ((a - b + 1) / b))
#define CEIL_DIV(a,b) (((a) + (b) - 1) / (b))

// For finding what chunks to load.

#define viewport_start_chunk_x FLOOR_DIV(camera_position_x, ((int32_t)TILE_SIZE * CHUNK_SIZE))
#define viewport_end_chunk_x CEIL_DIV(camera_position_x + (int32_t)framebuffer_size_x, ((int32_t)TILE_SIZE * CHUNK_SIZE))
#define viewport_start_chunk_y FLOOR_DIV(camera_position_y, ((int32_t)TILE_SIZE * CHUNK_SIZE))
#define viewport_end_chunk_y CEIL_DIV(camera_position_y + (int32_t)framebuffer_size_y, ((int32_t)TILE_SIZE * CHUNK_SIZE))

int resize_window();
void update_window_size(struct mfb_window *window, int width, int height);
void draw_chunk(int32_t cx, int32_t cy);
void update_tile_size();

extern GLFWwindow *window;
extern uint32_t *framebuffer;

extern uint32_t framebuffer_size_x;
extern uint32_t framebuffer_size_y;
extern uint32_t next_framebuffer_size_x;
extern uint32_t next_framebuffer_size_y;
extern uint32_t actual_window_size_x;
extern uint32_t actual_window_size_y;

extern int32_t camera_position_x;
extern int32_t camera_position_y;

extern uint32_t textures[];

// --------------------

typedef enum {
    FREE = 0,
    FULL = 1,
} loaded_chunk_pool_flags;

int resize_chunk_array();
int generate_chunk(int32_t x, int32_t y);
int load_nearby_chunks();

extern uint16_t *chunk_array;
extern uint8_t *chunk_flags;
extern int32_t *chunk_position_x;
extern int32_t *chunk_position_y;

extern uint32_t chunk_array_size; // In chunks!
extern uint32_t new_chunk_array_size;

int resize_spatial_access_grid();
uint32_t get_chunk_tiles(int32_t x, int32_t y);
void set_chunk_tiles(int32_t x, int32_t y, uint32_t index);
int set_tile(int32_t x, int32_t y, uint16_t tile_id);
uint32_t* get_chunk_nbt(int32_t x, int32_t y);
void set_chunk_nbt(int32_t x, int32_t y, uint32_t* index);

extern uint32_t **nbt_spatial_access_grid;
extern uint32_t *tile_spatial_access_grid;
extern uint32_t **nbt_spatial_access_grid;
extern int32_t grid_x, grid_y; // These coords refer to the top-left corner of the box in terms of world coordinates scaled to chunk units.
extern int32_t grid_w, grid_l; // In chunks!
// These variables are for updating the variables above.
extern int32_t new_grid_x, new_grid_y;
extern int32_t new_grid_w, new_grid_l;

// --------------------

#define REGION_WIDTH 16  // The horizontal size of a region in chunks
#define REGION_ELEMENT_COUNT REGION_WIDTH * REGION_WIDTH
#define MAX_TRACKED_POINTERS 256
#define MAX_MALLOC_RETRIES 0 // Not recommended to retry memory allocation upon failure.

void create_directory(const char *path);
void create_new_save(const char *path);
char* concatenate_strings(const char* a, const char* b);
int create_region(int32_t x, int32_t y);
void initialize_blank_region_header();

// Path to all saves
extern char *save_directory_path;
extern char *selected_save_directory;
extern uint32_t region_header_template[];

// Functions for tracking memory usage and performing proper error handling.
void initialize_tracked_memory_buffers();
void trigger_memory_failure();
void tracked_free(void* pointer);
void* tracked_malloc(uint32_t size);
void* tracked_realloc(void* pointer, uint32_t size);

extern uint32_t stack_memory_usage;
extern void*    pointer_stack[];
extern uint32_t buffer_sizes[];
extern uint32_t pointer_stack_top;

// -------------

void resize_entity_array();


extern uint32_t *entity_ids;
extern int32_t *entity_pos_x;
extern int32_t *entity_pos_y;

extern uint32_t current_entities;
extern uint32_t entity_pool_size;
extern uint32_t new_entity_pool_size;

extern uint32_t player_index;

// ------------

extern int keyboard[];
extern int mouse[];
extern double cursor_x;
extern double cursor_y;

void keyboard_event_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_event_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_event_callback(GLFWwindow* window, double xpos, double ypos);