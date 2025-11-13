#version 430 core
precision highp float;
in vec2 uv;
out vec4 FragColor;

uniform float camera_position_x;
uniform float camera_position_y;
uniform float camera_zoom;
uniform float aspect_ratio;

layout(std430, binding = 0) buffer ChunkData {
    uint tile_data[]; // Technically are packed 16-bit integers. Must use bit operations to extract from 32-bit.
};

uint get_tile_id(uint index) {
    uint pair = tile_data[index / 2u];
    if ((index & 1u) == 0u)
        return pair & 0xFFFFu;
    else
        return pair >> 16;
}

void main() {
    vec2 camera_position = vec2(camera_position_x, camera_position_y);
    
    // camera_pos + (uv - 0.5) * vec2(aspect * zoom, zoom)

    vec2 tile_coordinates = camera_position + (uv - 0.5) * vec2(aspect_ratio * camera_zoom, camera_zoom);

    int tile = int(mod(floor(tile_coordinates.x) + floor(tile_coordinates.y), 2.0));

    if (tile == 0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}
