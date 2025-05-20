#ifndef HEIGHTMAP_ENTITY_ELEMENT_H
#define HEIGHTMAP_ENTITY_ELEMENT_H

#include "SceneElement.h"
#include "scene/SceneContext.h"

namespace EditorScene {
class HeightmapEntityElement : virtual public SceneElement, public LocalTransformComponent, public LitMaterialComponent {
  public:
    /// NOTE: Must be unique per element type, as it is used to select generators,
    ///       so if you are creating a new element type make sure to change this to a new unique name.
    static constexpr const char *ELEMENT_TYPE_NAME = "Heightmap";

    float noise_scale = 1.0f;
    int resolution = 16;
    uint32_t seed = 0;
    bool need_update_model = false;
    bool need_update_noise = false;

    std::shared_ptr<HeightmapEntityRenderer::Entity> rendered_entity;

    HeightmapEntityElement(const ElementRef &parent, std::string name, const glm::vec3 &position, const glm::vec3 &euler_rotation, const glm::vec3 &scale, std::shared_ptr<HeightmapEntityRenderer::Entity> rendered_entity) : SceneElement(parent, std::move(name)), LocalTransformComponent(position, euler_rotation, scale), LitMaterialComponent(rendered_entity->instance_data.material), rendered_entity(std::move(rendered_entity)) {}

    static std::unique_ptr<HeightmapEntityElement> new_default(const SceneContext &scene_context, ElementRef parent);
    static std::unique_ptr<HeightmapEntityElement> from_json(const SceneContext &scene_context, ElementRef parent, const json &j);
    [[nodiscard]] json into_json() const override;

    void add_imgui_edit_section(MasterRenderScene &render_scene, const SceneContext &scene_context) override;

    void update_instance_data() override;

    void add_to_render_scene(MasterRenderScene &target_render_scene) override {
        target_render_scene.insert_entity(rendered_entity);
    }

    void remove_from_render_scene(MasterRenderScene &target_render_scene) override {
        target_render_scene.remove_entity(rendered_entity);
    }

    [[nodiscard]] const char *element_type_name() const override;
};
} // namespace EditorScene

#endif // HEIGHTMAP_ENTITY_ELEMENT_H
