#ifndef HEIGHTMAP_ENTITY_RENDERER_H
#define HEIGHTMAP_ENTITY_RENDERER_H

#include <unordered_set>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "rendering/memory/UniformBufferArray.h"
#include "rendering/renders/EntityRenderer.h"
#include "rendering/renders/shaders/HeightmapLitEntityShader.h"
#include "rendering/renders/shaders/ShaderInterface.h"
#include "rendering/resources/ModelLoader.h"
#include "rendering/resources/TextureHandle.h"
#include "rendering/scene/GlobalData.h"
#include "rendering/scene/Lights.h"
#include "rendering/scene/RenderScene.h"
#include "rendering/scene/RenderedEntity.h"

#include "EntityRenderer.h"
#include "rendering/renders/shaders/BaseLitEntityShader.h"

namespace HeightmapEntityRenderer {
// struct VertexData {
//     glm::vec3 position;
//     glm::vec3 normal;
//     glm::vec2 texture_coordinate;

//     static void from_mesh(const VertexCollection &vertex_collection, std::vector<VertexData> &out_vertices);
//     static void setup_attrib_pointers();
// };
using VertexData = EntityRenderer::VertexData;

using EntityMaterial = BaseLitEntityMaterial;
using InstanceData = BaseLitEntityInstanceData;
using GlobalData = BaseLitEntityGlobalData;
using RenderData = HeightmapEntityRenderData;
// using RenderData = BaseLitEntityRenderData;

using Entity = RenderedEntity<VertexData, InstanceData, RenderData>;

using RenderScene = RenderScene<Entity, GlobalData>;

class EntityShader : public BaseHeightmapEntityShader {
    int normal_matrix_location{};

  public:
    EntityShader();

    void set_instance_data(const BaseLitEntityInstanceData &instance_data);

  protected:
    void get_uniforms_set_bindings() override;
};

class EntityRenderer {
    EntityShader shader;

  public:
    EntityRenderer();

    void render(const RenderScene &render_scene, const LightScene &light_scene);

    bool refresh_shaders();
};
} // namespace HeightmapEntityRenderer

#endif // ENTITY_RENDERER_H
