#include "DirectionalLightElement.h"

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/transform.hpp>

#include "glm/gtx/quaternion.hpp"
#include "rendering/imgui/ImGuiManager.h"
#include "scene/SceneContext.h"

std::unique_ptr<EditorScene::DirectionalLightElement> EditorScene::DirectionalLightElement::new_default(const SceneContext &scene_context, EditorScene::ElementRef parent) {
    auto light_element = std::make_unique<DirectionalLightElement>(
        parent,
        "New Directional Light",
        glm::vec3{0.0f, 1.0f, 0.0f},
        DirectionalLight::create(
            glm::quat{}, // Set via update_instance_data()
            glm::vec4{1.0f}),
        EmissiveEntityRenderer::Entity::create(
            scene_context.model_loader.load_from_file<EmissiveEntityRenderer::VertexData>("cone.obj"),
            EmissiveEntityRenderer::InstanceData{
                glm::mat4{}, // Set via update_instance_data()
                EmissiveEntityRenderer::EmissiveEntityMaterial{
                    glm::vec4{1.0f}}},
            EmissiveEntityRenderer::RenderData{
                scene_context.texture_loader.default_white_texture()}));

    light_element->update_instance_data();
    return light_element;
}

std::unique_ptr<EditorScene::DirectionalLightElement> EditorScene::DirectionalLightElement::from_json(const SceneContext &scene_context, EditorScene::ElementRef parent, const json &j) {
    auto light_element = new_default(scene_context, parent);

    light_element->position = j["position"];
    light_element->light->colour = j["colour"];
    light_element->visible = j["visible"];
    light_element->visual_scale = j["visual_scale"];
    light_element->euler_angles = j["euler_angles"];

    light_element->update_instance_data();
    return light_element;
}

json EditorScene::DirectionalLightElement::into_json() const {
    return {
        {"position", position},
        {"colour", light->colour},
        {"visible", visible},
        {"visual_scale", visual_scale},
        {"euler_angles", euler_angles},
    };
}

void EditorScene::DirectionalLightElement::add_imgui_edit_section(MasterRenderScene &render_scene, const SceneContext &scene_context) {
    ImGui::Text("Point Light");
    SceneElement::add_imgui_edit_section(render_scene, scene_context);

    ImGui::Text("Local Transformation");
    bool transformUpdated = false;
    transformUpdated |= ImGui::DragFloat3("Translation", &position[0], 0.01f);
    ImGui::DragDisableCursor(scene_context.window);
    transformUpdated |= ImGui::DragFloat3("Rotation", &euler_angles[0], 0.01f);
    ImGui::DragDisableCursor(scene_context.window);
    ImGui::Spacing();

    ImGui::Text("Light Properties");
    transformUpdated |= ImGui::ColorEdit3("Colour", &light->colour[0]);
    ImGui::Spacing();
    ImGui::DragFloat("Intensity", &light->colour.a, 0.01f, 0.0f, FLT_MAX);
    ImGui::DragDisableCursor(scene_context.window);

    ImGui::Spacing();
    ImGui::Text("Visuals");

    transformUpdated |= ImGui::Checkbox("Show Visuals", &visible);
    transformUpdated |= ImGui::DragFloat("Visual Scale", &visual_scale, 0.01f, 0.0f, FLT_MAX);
    ImGui::DragDisableCursor(scene_context.window);

    if (transformUpdated) {
        update_instance_data();
    }
}

void EditorScene::DirectionalLightElement::update_instance_data() {
    transform = glm::translate(position) * glm::rotate(euler_angles.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(euler_angles.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(euler_angles.z, glm::vec3(0.0f, 0.0f, 1.0f));

    if (!EditorScene::is_null(parent)) {
        // Post multiply by transform so that local transformations are applied first
        transform = (*parent)->transform * transform;
    }

    light->rotation = glm::toQuat(transform);
    if (visible) {
        light_sphere->instance_data.model_matrix = transform * glm::scale(glm::vec3{0.1f * visual_scale});
    } else {
        // Throw off to infinity as a hacky way to make model invisible
        light_sphere->instance_data.model_matrix = glm::scale(glm::vec3{std::numeric_limits<float>::infinity()}) * glm::translate(glm::vec3{std::numeric_limits<float>::infinity()});
    }

    glm::vec3 normalised_colour = glm::vec3(light->colour) / glm::compMax(glm::vec3(light->colour));
    light_sphere->instance_data.material.emission_tint = glm::vec4(normalised_colour, light_sphere->instance_data.material.emission_tint.a);
}

const char *EditorScene::DirectionalLightElement::element_type_name() const {
    return ELEMENT_TYPE_NAME;
}
