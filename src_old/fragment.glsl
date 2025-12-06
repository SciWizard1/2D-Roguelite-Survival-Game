#version 430 core
precision highp float;
in vec2 uv;
out vec4 FragColor;

#define CHUNK_SIZE 64
#define CHUNK_SIZE_FLOAT 64.0
#define CHUNK_BSHIFT 6
#define CHUNK_MASK 0x3F

uniform float camera_position_x;
uniform float camera_position_y;
uniform float camera_zoom;
uniform float aspect_ratio;

uniform uint chunk_array_size;

uniform int spatial_access_grid_x;
uniform int spatial_access_grid_y;
uniform int spatial_access_grid_w;
uniform int spatial_access_grid_l;

uniform usampler2DArray chunk_array;
uniform sampler2DArray tile_textures;

struct chunk_entry {
    int  chunk_array_index;
    int  chunk_status;
    uint nbt_ptr_low;
    uint nbt_ptr_high;

    int _pad0;
    int _pad1;
    int _pad2;
    int _pad3;
};

layout(std140, binding = 0) uniform sag {
    chunk_entry spatial_access_grid[64];
};

int signed_rshift(int a, int shift) {
    return (a + ((a >> 31) & ((1 << shift) - 1))) >> shift;
}

void main() {
    vec2 camera_position = vec2(camera_position_x, camera_position_y);

    // convert pixel uv to world tile coords
    vec2 world = camera_position + (uv - 0.5) * vec2(aspect_ratio * camera_zoom, camera_zoom);
    ivec2 tile = ivec2(floor(world));
    vec2 tile_uv = fract(world);
    ivec2 chunk = ivec2(floor(world / CHUNK_SIZE_FLOAT));

    bool is_valid_chunk = chunk.x >= spatial_access_grid_x && chunk.x < spatial_access_grid_x + spatial_access_grid_w &&
                          chunk.y >= spatial_access_grid_y && chunk.y < spatial_access_grid_y + spatial_access_grid_l;
    
    // Still need to fix artifact that occurs when loading a new chunk.
    if(!is_valid_chunk) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    int sag_index = (chunk.x - spatial_access_grid_x) + 
                    (chunk.y - spatial_access_grid_y) * spatial_access_grid_w;

    // lookup the chunk and intra-chunk tile coords
    int chunk_index = spatial_access_grid[sag_index].chunk_array_index;
    int chunk_status = spatial_access_grid[sag_index].chunk_status;

    //is_valid_chunk = chunk_status == 1;
    //if (!is_valid_chunk) {
    //    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    //    return;
    //}

    ivec2 local = ivec2(mod(tile.x, CHUNK_SIZE), mod(tile.y, CHUNK_SIZE));

    // read the tile ID
    uvec4 tile_id_vec4 = texelFetch(chunk_array, ivec3(local.x, local.y, chunk_index), 0);
    uint tile_id = tile_id_vec4.r;

    // sample tile sprite
    FragColor = texture(tile_textures, vec3(tile_uv, int(tile_id)));
    //FragColor = vec4(local / CHUNK_SIZE_FLOAT, 0.0, 1.0);
}