#include "HeightmapEntityElement.h"

#include "feature/simplex.hpp"
#include "imgui.h"
#include "rendering/imgui/ImGuiManager.h"
#include "rendering/renders/HeightmapEntityRenderer.h"
#include "rendering/resources/ModelLoader.h"
#include "rendering/resources/TextureHandle.h"
#include "scene/SceneContext.h"
#include <cstdint>
#include <memory>

std::shared_ptr<ModelHandle<HeightmapEntityRenderer::VertexData>> generate_model(const uint size);
void gen_noise_texture(TextureHandle &texture, uint resolution, float noise_scale, uint32_t seed);

std::unique_ptr<EditorScene::HeightmapEntityElement> EditorScene::HeightmapEntityElement::new_default(const SceneContext &scene_context, ElementRef parent) {
    auto rendered_entity = HeightmapEntityRenderer::Entity::create(
        generate_model(16),
        HeightmapEntityRenderer::InstanceData{
            glm::mat4{}, // Set via update_instance_data()
            HeightmapEntityRenderer::EntityMaterial{
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 1.0f},
                512.0f,
            },
        },
        HeightmapEntityRenderer::RenderData{
            scene_context.texture_loader.default_white_texture(),
            scene_context.texture_loader.default_white_texture(),
        });
    gen_noise_texture(*rendered_entity->render_data.heightmap_texture, 16, 1.0f, 0);

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

std::shared_ptr<ModelHandle<HeightmapEntityRenderer::VertexData>> generate_model(const uint size) {
    std::vector<HeightmapEntityRenderer::VertexData> vertices(size * size);
    for (int i = 0; i < size * size; ++i) {
        int x = i % size;
        int y = i / size;
        float u = float(x) / size;
        float v = float(y) / size;
        vertices[i] = {{x, y, 0.}, {0., 0., 1.}, {u, v}};
    }
    std::vector<uint> indices((size - 1) * (size - 1) * 2 * 3);
    for (int i = 0; i < (size - 1) * (size - 1); ++i) {
        int x = i % (size - 1);
        int y = i / (size - 1);
        int vertex_index = x + y * size;
        // tri 1
        indices[i * 6 + 0] = vertex_index;
        indices[i * 6 + 1] = vertex_index + 1;
        indices[i * 6 + 3] = vertex_index + size;
        // tri 2
        indices[i * 6 + 3] = vertex_index + size + 1;
        indices[i * 6 + 4] = vertex_index + 1;
        indices[i * 6 + 5] = vertex_index + size;
    }
    std::cout << vertices.size() << ' ' << indices.size() << '\n';
    return ModelLoader::load_from_data(vertices, indices, std::nullopt);
}

void gen_noise_texture(TextureHandle &texture, uint resolution, float noise_scale, uint32_t seed) {
    std::vector<float> pixels(resolution * resolution);
    for (uint i = 0; i < resolution * resolution; ++i) {
        pixels[i] = Simplex::generate(double(i % resolution) * noise_scale, double(uint(i / resolution)) * noise_scale, seed);
    }
    glBindTexture(GL_TEXTURE_2D, texture.texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution, resolution, 0, GL_RED, GL_FLOAT, pixels.data());
    texture.width = texture.height = resolution;
    texture.srgb = false;
}

std::unique_ptr<EditorScene::HeightmapEntityElement> EditorScene::HeightmapEntityElement::from_json(const SceneContext &scene_context, EditorScene::ElementRef parent, const json &j) {
    auto new_entity = new_default(scene_context, parent);

    new_entity->update_local_transform_from_json(j);
    new_entity->update_material_from_json(j);

    new_entity->noise_scale = j["noise_scale"];
    new_entity->resolution = j["resolution"];
    new_entity->seed = j["seed"];
    new_entity->rendered_entity->model = generate_model(new_entity->resolution);
    new_entity->rendered_entity->render_data.diffuse_texture = texture_from_json(scene_context, j["diffuse_texture"]);
    new_entity->rendered_entity->render_data.specular_map_texture = texture_from_json(scene_context, j["specular_map_texture"]);
    gen_noise_texture(*new_entity->rendered_entity->render_data.heightmap_texture, new_entity->resolution, new_entity->noise_scale, new_entity->seed);
    new_entity->rendered_entity->instance_data.texture_scale = j["texture_scale"];

    new_entity->update_instance_data();
    return new_entity;
}

json EditorScene::HeightmapEntityElement::into_json() const {
    return {
        local_transform_into_json(),
        material_into_json(),
        {"noise_scale", noise_scale},
        {"resolution", resolution},
        {"seed", seed},
        // {"model", rendered_entity->model->get_filename().value()},
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
    need_update_noise |= ImGui::DragFloat("Noise scale", &noise_scale);
    need_update_noise |= ImGui::InputInt("Noise seed", reinterpret_cast<int32_t *>(&seed));
    need_update_model |= need_update_noise |= ImGui::DragInt("Resolution", &resolution);
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
    if (need_update_model || need_update_noise)
        update_instance_data();
}

void EditorScene::HeightmapEntityElement::update_instance_data() {
    transform = calc_model_matrix();

    if (!EditorScene::is_null(parent)) {
        // Post multiply by transform so that local transformations are applied first
        transform = (*parent)->transform * transform;
    }
    if (need_update_noise) {
        gen_noise_texture(*rendered_entity->render_data.heightmap_texture, resolution, noise_scale, seed);
        need_update_noise = false;
    }
    if (need_update_model) {
        rendered_entity->model = generate_model(resolution);
        std::cout << rendered_entity->model->get_vao() << '\n';
        need_update_model = false;
    }

    rendered_entity->instance_data.model_matrix = transform;
    rendered_entity->instance_data.material = material;
}

const char *EditorScene::HeightmapEntityElement::element_type_name() const {
    return ELEMENT_TYPE_NAME;
}
