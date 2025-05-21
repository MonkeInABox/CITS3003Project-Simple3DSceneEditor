#include "HeightmapEntityElement.h"

#include "../../feature/generate.hpp"
#include "imgui.h"
#include "rendering/imgui/ImGuiManager.h"
#include "scene/SceneContext.h"

std::unique_ptr<EditorScene::HeightmapEntityElement> EditorScene::HeightmapEntityElement::new_default(const SceneContext &scene_context, ElementRef parent) {
    auto rendered_entity = HeightmapEntityRenderer::Entity::create(
        // scene_context.model_loader.load_from_file<EntityRenderer::VertexData>("cube.obj"),
        generate_model(16),
        EntityRenderer::InstanceData{
            glm::mat4{}, // Set via update_instance_data()
            EntityRenderer::EntityMaterial{
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 1.0f},
                512.0f,
            },
        },
        HeightmapEntityRenderer::RenderData{
            scene_context.texture_loader.default_white_texture(),
            // create_noise_texture(64, 1.f, 0),
            scene_context.texture_loader.default_white_texture()});

    auto new_entity = std::make_unique<HeightmapEntityElement>(
        parent,
        "New Heightmap",
        glm::vec3{0.0f},
        glm::vec3{0.0f},
        glm::vec3{1.0f},
        rendered_entity);

    new_entity->update_instance_data();
    return new_entity;
}

std::unique_ptr<EditorScene::HeightmapEntityElement> EditorScene::HeightmapEntityElement::from_json(const SceneContext &scene_context, EditorScene::ElementRef parent, const json &j) {
    auto new_entity = new_default(scene_context, parent);

    new_entity->update_local_transform_from_json(j);
    new_entity->update_material_from_json(j);

    new_entity->rendered_entity->model = scene_context.model_loader.load_from_file<EntityRenderer::VertexData>(j["model"]);
    new_entity->rendered_entity->render_data.diffuse_texture = texture_from_json(scene_context, j["diffuse_texture"]);
    new_entity->rendered_entity->render_data.specular_map_texture = texture_from_json(scene_context, j["specular_map_texture"]);
    new_entity->rendered_entity->instance_data.texture_scale = j["texture_scale"];

    new_entity->update_instance_data();
    return new_entity;
}

json EditorScene::HeightmapEntityElement::into_json() const {
    if (!rendered_entity->model->get_filename().has_value()) {
        return {
            {"error", Formatter() << "Entity [" << name << "]'s model does not have a filename so can not be exported, and has been skipped."}};
    }

    return {
        local_transform_into_json(),
        material_into_json(),
        {"model", rendered_entity->model->get_filename().value()},
        {"diffuse_texture", texture_to_json(rendered_entity->render_data.diffuse_texture)},
        {"specular_map_texture", texture_to_json(rendered_entity->render_data.specular_map_texture)},
        {"texture_scale", rendered_entity->instance_data.texture_scale},
    };
}

void EditorScene::HeightmapEntityElement::add_imgui_edit_section(MasterRenderScene &render_scene, const SceneContext &scene_context) {
    ImGui::Text("Entity");
    SceneElement::add_imgui_edit_section(render_scene, scene_context);

    add_local_transform_imgui_edit_section(render_scene, scene_context);
    add_material_imgui_edit_section(render_scene, scene_context);

    ImGui::Text("Heightmap");
    ImGui::Text("Textures");
    scene_context.texture_loader.add_imgui_texture_selector("Diffuse Texture", rendered_entity->render_data.diffuse_texture);
    scene_context.texture_loader.add_imgui_texture_selector("Specular Map", rendered_entity->render_data.specular_map_texture, false);
    ImGui::Text("Material");
    ImGui::ColorEdit3("Diffuse Tint", &rendered_entity->instance_data.material.diffuse_tint.r);
    ImGui::DragFloat("Diffuse Factor", &rendered_entity->instance_data.material.diffuse_tint.a, 0.05);
    ImGui::ColorEdit3("Specular Tint", &rendered_entity->instance_data.material.specular_tint.r);
    ImGui::DragFloat("Specular Factor", &rendered_entity->instance_data.material.specular_tint.a, 0.05);
    ImGui::ColorEdit3("Ambient Tint", &rendered_entity->instance_data.material.ambient_tint.r);
    ImGui::DragFloat("Ambient Factor", &rendered_entity->instance_data.material.ambient_tint.a, 0.05);
    ImGui::DragFloat("Shininess", &rendered_entity->instance_data.material.shininess, 1.0f, 0.0f, FLT_MAX);
    ImGui::DragFloat2("Texture Scale", &rendered_entity->instance_data.texture_scale.x, 0.05);
    ImGui::Spacing();
}

void EditorScene::HeightmapEntityElement::update_instance_data() {
    transform = calc_model_matrix();

    if (!EditorScene::is_null(parent)) {
        // Post multiply by transform so that local transformations are applied first
        transform = (*parent)->transform * transform;
    }

    rendered_entity->instance_data.model_matrix = transform;
    rendered_entity->instance_data.material = material;
}

const char *EditorScene::HeightmapEntityElement::element_type_name() const {
    return ELEMENT_TYPE_NAME;
}
