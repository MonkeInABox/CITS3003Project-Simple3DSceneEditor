#version 410 core
// #include "../common/lights.glsl"

// Per vertex data
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;

out VertexOut {
    vec3 position;
    vec3 view_dir;
    vec3 normal;
    vec2 texture_coordinate;
} vertex_out;

// Per instance data
uniform mat4 model_matrix;
uniform mat3 normal_matrix;

uniform sampler2D heightmap_texture;

// Global data
uniform vec3 ws_view_position;
uniform mat4 projection_view_matrix;

void main() {
    // float height_sample = texture(heightmap_texture, texture_coordinate);
    float height_sample = 0;
    // Transform vertices
    vec3 ws_position = (model_matrix * vec4(vertex_position + vec3(0,0,height_sample), 1.0f)).xyz;
    vertex_out.position = ws_position;
    vertex_out.normal = normalize(normal_matrix * normal);
    vertex_out.texture_coordinate = texture_coordinate;

    gl_Position = projection_view_matrix * vec4(ws_position, 1.0f);

    // Per vertex lighting
    vertex_out.view_dir = normalize(ws_view_position - ws_position);
    // LightCalculatioData light_calculation_data = LightCalculatioData(ws_position, ws_view_dir, ws_normal);
    // Material material = Material(diffuse_tint, specular_tint, ambient_tint, shininess);

    // vertex_out.lighting_result = total_light_calculation(light_calculation_data, material
    //     #if NUM_PL > 0
    //     ,point_lights
    //     #endif
    // );
}
