#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MiniFB.h"

// Math Utilities

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define FLOOR_DIV(a,b) ((a >= 0) ? (a / b) : ((a - b + 1) / b))
#define CEIL_DIV(a,b) (((a) + (b) - 1) / (b))

extern GLuint shader_program;

// baserenderer.c

// camera_pos + (uv - 0.5) * vec2(aspect * zoom, zoom)
// camera_pos - 0.5 * vec2(aspect * zoom, zoom)

#define WINDOW_NAME "The Wonderous Deathly Valley"
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_WINDOW_WIDTH 800

#define SHADER_COMPILATION_LOG_SIZE 512
#define SHADER_BUFFER_MAXIMUM_FRAMES 2

void update_tile_size();
int initialize_window_context();
int setup_vertex_buffers();
GLuint compile_shader(const char* source, GLenum type);
GLuint link_shaders();
void update_viewport();

extern GLuint shader_chunk_array_texture;

extern GLFWwindow *window;
extern const float screen_quad[];
extern uint32_t *framebuffer;

extern uint32_t framebuffer_size_x;
extern uint32_t framebuffer_size_y;
extern uint32_t next_framebuffer_size_x;
extern uint32_t next_framebuffer_size_y;
extern uint32_t actual_window_size_x;
extern uint32_t actual_window_size_y;

extern int32_t window_width;
extern int32_t window_height;
extern float aspect_ratio;

extern float camera_position_x;
extern float camera_position_y;
extern float camera_zoom;

extern float viewport_start_chunk_x;
extern float viewport_end_chunk_x;
extern float viewport_start_chunk_y;
extern float viewport_end_chunk_y;

extern uint32_t textures[];
extern uint32_t num_textures;

extern uint32_t TILE_SIZE_EXPONENT; // In powers of two
extern uint32_t TILE_SIZE;
extern uint32_t TILE_MASK;

extern GLuint vertex_array_object;
extern GLuint vertex_buffer_object;
extern GLuint element_buffer_object;

// shaders_embedded.c

extern const char* vertex_src;
extern const char* fragment_src;

// chunks.c

// Chunk and tile information.
// Masks are for running modulo operators using the AND bitwise operator.
// Ensure CHUNK_SIZE is defined as the same value in the fragment shader!
#define CHUNK_SIZE 64
#define CHUNK_MASK (CHUNK_SIZE - 1)
#define CHUNK_TEXTURE_BYTES CHUNK_SIZE * CHUNK_SIZE * sizeof(uint16_t)

typedef enum {
    FREE = 0,
    FULL = 1,
} loaded_chunk_pool_flags;

typedef enum {
    NULL_CHUNK = 0,
    LOADED     = 1,
    DISK       = 2,
} chunk_status_enum;

typedef struct {
    int32_t   chunk_array_index;
    int32_t   chunk_status; // Technically should be of type chunk_status_enum, but this is not explicitly defined to be uint32_t.
    uint32_t* nbt_instance;
} chunk_entry;

// Aligned to 32 bytes for std140.
typedef struct {
    int32_t   chunk_array_index;
    int32_t   chunk_status; // Technically should be of type chunk_status_enum, but this is not explicitly defined to be uint32_t.
    uint32_t* nbt_instance;

    int32_t _pad0;
    int32_t _pad1;
    int32_t _pad2;
    int32_t _pad3;
} chunk_entry_padded;


int resize_chunk_array();
int generate_chunk(int32_t x, int32_t y);
int load_nearby_chunks();

extern uint16_t *chunk_array;
extern uint8_t *chunk_flags;
extern int32_t *chunk_position_x;
extern int32_t *chunk_position_y;

extern uint32_t chunk_array_size;
extern uint32_t new_chunk_array_size;

int resize_spatial_access_grid();
void update_gpu_spatial_access_grid();
chunk_entry get_chunk(int32_t x, int32_t y);
void set_chunk_tiles(int32_t x, int32_t y, uint32_t index);
void set_chunk_nbt(int32_t x, int32_t y, uint32_t* index);
void set_chunk_status(int32_t x, int32_t y, chunk_status_enum status);
int set_tile(int32_t x, int32_t y, uint16_t tile_id);

extern chunk_entry *spatial_access_grid;
extern int32_t grid_x, grid_y;
extern int32_t grid_w, grid_l;

extern int32_t new_grid_x, new_grid_y;
extern int32_t new_grid_w, new_grid_l;

extern chunk_entry_padded *gpu_spatial_access_grid;

extern int32_t gpu_grid_x, gpu_grid_y;
extern int32_t gpu_grid_w, gpu_grid_l;

// save_file_manager.c

#define REGION_WIDTH 16
#define REGION_ELEMENT_COUNT REGION_WIDTH * REGION_WIDTH
#define MAX_TRACKED_POINTERS 256
#define MAX_MALLOC_RETRIES 0 // Not recommended to retry memory allocation upon failure.

void create_directory(const char *path);
void create_new_save(const char *path);
int create_region(int32_t x, int32_t y);
void initialize_blank_region_header();
void* read_file_into_buffer(char* path);

extern char *save_directory_path;
extern char *selected_save_directory;
extern uint32_t region_header_template[];

// string_utilities.c

char* concatenate_strings(const char* a, const char* b);

void initialize_tracked_memory_buffers();
void trigger_memory_failure();
void tracked_free(void* pointer);
void* tracked_malloc(uint32_t size);
void* tracked_realloc(void* pointer, uint32_t size);

extern uint32_t stack_memory_usage;
extern void*    pointer_stack[];
extern uint32_t buffer_sizes[];
extern uint32_t pointer_stack_top;

// entities.c

void resize_entity_array();


extern uint32_t *entity_ids;
extern float *entity_pos_x;
extern float *entity_pos_y;

extern uint32_t current_entities;
extern uint32_t entity_pool_size;
extern uint32_t new_entity_pool_size;

extern uint32_t player_index;

// controls.c

#define KEY_COUNT (GLFW_KEY_LAST + 1)
#define MOUSE_BUTTON_COUNT (GLFW_MOUSE_BUTTON_LAST + 1)

extern int keyboard[];
extern int mouse[];
extern double cursor_x;
extern double cursor_y;

void keyboard_event_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_event_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_event_callback(GLFWwindow* window, double xpos, double ypos);

// gpu_data_transfer.c

typedef struct {
    GLuint  descriptor;
    int32_t binding_index;
} texture_instance;

texture_instance generate_storage_texture_array(char *shader_uniform_name, int width, int height, int array_size);
texture_instance generate_graphics_texture_array(char *shader_uniform_name, int width, int height, int array_size);
void update_chunk_texture(texture_instance chunk_texture_instance, int index);