#version 410 core
#include "../common/lights.glsl"

in VertexOut {
    vec3 position;
    vec3 view_dir;
    vec3 normal;
    vec2 texture_coordinate;
} frag_in;

layout(location = 0) out vec4 out_colour;

// Light Data
#if NUM_PL > 0
layout (std140) uniform PointLightArray {
    PointLightData point_lights[NUM_PL];
};
#endif

#if NUM_DL > 0
layout (std140) uniform DirectionalLightArray {
    DirectionalLightData directional_lights[NUM_DL];
};
#endif

// Material properties
uniform vec3 diffuse_tint;
uniform vec3 specular_tint;
uniform vec3 ambient_tint;
uniform float shininess;

// Global Data
uniform float inverse_gamma;

uniform vec2 texture_scale;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_map_texture;

void main() {
    LightCalculatioData light_calculation_data = LightCalculatioData(frag_in.position, frag_in.view_dir, frag_in.normal);
    Material material = Material(diffuse_tint, specular_tint, ambient_tint, shininess);

    LightingResult lighting_result = total_light_calculation(light_calculation_data, material
        #if NUM_PL > 0
        ,point_lights
        #endif
        #if NUM_DL > 0
        ,directional_lights
        #endif
    );

    vec3 resolved_lighting = resolve_textured_light_calculation(lighting_result, diffuse_texture, specular_map_texture, frag_in.texture_coordinate * texture_scale);

    out_colour = vec4(resolved_lighting, 1.0f);
    out_colour.rgb = pow(out_colour.rgb, vec3(inverse_gamma));
}
