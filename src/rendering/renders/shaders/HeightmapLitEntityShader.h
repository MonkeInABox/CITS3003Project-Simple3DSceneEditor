#ifndef HEIGHTMAP_LIT_ENTITY_SHADER_H
#define HEIGHTMAP_LIT_ENTITY_SHADER_H

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"

#include "ShaderInterface.h"
#include "rendering/memory/UniformBufferArray.h"
#include "rendering/resources/ModelLoader.h"
#include "rendering/resources/TextureHandle.h"
#include "rendering/scene/GlobalData.h"
#include "rendering/scene/Lights.h"
#include "rendering/scene/RenderScene.h"
#include "rendering/scene/RenderedEntity.h"

// #include "BaseEntityShader.h"
#include "BaseLitEntityShader.h"
#include "utility/HelperTypes.h"

// struct BaseLitEntityMaterial {
//     // Alpha components are just used to store a scalar that is applied before passing to the GPU
//     glm::vec4 diffuse_tint;
//     glm::vec4 specular_tint;
//     glm::vec4 ambient_tint;
//     float shininess;
// };

struct HeightmapLitEntityInstanceData : public BaseLitEntityInstanceData {
    HeightmapLitEntityInstanceData(const glm::mat4 &model_matrix, const BaseLitEntityMaterial &material) : BaseLitEntityInstanceData(model_matrix, material) {}

    // Material properties
    // BaseLitEntityMaterial material;
    // glm::vec2 texture_scale = {1.f, 1.f};
};

struct HeightmapLitEntityRenderData : public BaseLitEntityRenderData {
    HeightmapLitEntityRenderData(std::shared_ptr<TextureHandle> diffuse_texture, std::shared_ptr<TextureHandle> specular_map_texture);
    std::shared_ptr<TextureHandle> heightmap_texture;
};

using HeightmapLitEntityGlobalData = BaseEntityGlobalData;

class HeightmapLitEntityShader : public BaseLitEntityShader {
    // public:
    //   static constexpr uint MAX_PL = 16;
    //   static constexpr uint MAX_DL = 8;

  protected:
    // Material
    // int diffuse_tint_location{};
    // int specular_tint_location{};
    // int ambient_tint_location{};
    // int shininess_location{};
    // int texture_scale_location{};

    // static const uint POINT_LIGHT_BINDING = 0;
    // static const uint DIRECTIONAL_LIGHT_BINDING = 1;

    // UniformBufferArray<PointLight::Data, MAX_PL> point_lights_ubo;
    // UniformBufferArray<DirectionalLight::Data, MAX_DL> directional_lights_ubo;

  public:
    HeightmapLitEntityShader(std::string name, const std::string &vertex_path, const std::string &fragment_path,
                             std::unordered_map<std::string, std::string> vert_defines = {},
                             std::unordered_map<std::string, std::string> frag_defines = {});

    void set_instance_data(const HeightmapLitEntityInstanceData &instance_data);

  protected:
    void get_uniforms_set_bindings() override;
};

#endif // HEIGHTMAP_LIT_ENTITY_SHADER_H
