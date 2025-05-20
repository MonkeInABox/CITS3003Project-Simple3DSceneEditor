#include "HeightmapLitEntityShader.h"
#include "glad/gl.h"
#include "glm/fwd.hpp"
#include "rendering/resources/TextureHandle.h"
#include "rendering/scene/Lights.h"

#include <utility>

HeightmapLitEntityRenderData::HeightmapLitEntityRenderData(std::shared_ptr<TextureHandle> diffuse_texture, std::shared_ptr<TextureHandle> specular_map_texture) : BaseLitEntityRenderData(diffuse_texture, specular_map_texture) {
    constexpr int width = 1, height = 1;
    float temp[width * height] = {};
    uint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, temp);
    heightmap_texture = std::make_shared<TextureHandle>(tex_id, width, height, false, false);
}

HeightmapLitEntityShader::HeightmapLitEntityShader(std::string name, const std::string &vertex_path, const std::string &fragment_path,
                                                   std::unordered_map<std::string, std::string> vert_defines,
                                                   std::unordered_map<std::string, std::string> frag_defines)
    : BaseLitEntityShader(std::move(name), vertex_path, fragment_path, std::move(vert_defines), std::move(frag_defines)) {
    get_uniforms_set_bindings();
}

void HeightmapLitEntityShader::get_uniforms_set_bindings() {
    BaseLitEntityShader::get_uniforms_set_bindings(); // Call the base implementation to load all the common uniforms
    // Texture sampler bindings
    set_binding("heightmap_texture", 3);
}

void HeightmapLitEntityShader::set_instance_data(const HeightmapLitEntityInstanceData &instance_data) {
    BaseLitEntityShader::set_instance_data(instance_data);
}