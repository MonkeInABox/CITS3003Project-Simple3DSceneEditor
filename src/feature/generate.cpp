#include "generate.hpp"
#include "glad/gl.h"
#include "rendering/resources/TextureHandle.h"
#include "simplex.hpp"
#include <memory>

std::shared_ptr<ModelHandle<HeightmapEntityRenderer::VertexData>> generate_model(const uint size) {
    std::vector<HeightmapEntityRenderer::VertexData> vertices(size * size);
    float center = size / 2.f;
    for (int i = 0; i < size * size; ++i) {
        int x = i % size;
        int y = i / size;
        float u = float(x) / size;
        float v = float(y) / size;
        vertices[i] = {
            {x - center, 0., y - center},
            {0., 1., 0.}, // this results in bad looking lighting but i dont have time to figure out how to calculate + implement normal vectors of a heightmap
            {u, v}};
    }
    std::vector<uint> indices((size - 1) * (size - 1) * 2 * 3);
    for (int i = 0; i < (size - 1) * (size - 1); ++i) {
        int x = i % (size - 1);
        int y = i / (size - 1);
        int vertex_index = x + y * size;
        // tri 1
        indices[i * 6 + 0] = vertex_index + size;
        indices[i * 6 + 1] = vertex_index + 1;
        indices[i * 6 + 2] = vertex_index;
        // tri 2
        indices[i * 6 + 3] = vertex_index + size + 1;
        indices[i * 6 + 4] = vertex_index + 1;
        indices[i * 6 + 5] = vertex_index + size;
    }
    return ModelLoader::load_from_data(vertices, indices, std::nullopt);
}

void gen_noise_texture(uint texture_id, uint resolution, float noise_scale, uint32_t seed) {
    std::vector<float> pixels(resolution * resolution);
    for (uint i = 0; i < resolution * resolution; ++i) {
        pixels[i] = Simplex::generate(double(i % resolution) * noise_scale, double(uint(i / resolution)) * noise_scale, seed); // * 0.5 + 0.5;
    }
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, resolution, resolution);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, resolution, resolution, GL_RED, GL_FLOAT, pixels.data());
}
void gen_noise_texture(TextureHandle &texture, uint resolution, float noise_scale, uint32_t seed) {
    gen_noise_texture(texture.texture_id, resolution, noise_scale, seed);
    texture.width = texture.height = resolution;
    texture.srgb = false;
}

std::shared_ptr<TextureHandle> create_noise_texture(uint resolution, float noise_scale, uint32_t seed) {
    uint tex_id;
    glGenTextures(1, &tex_id);
    gen_noise_texture(tex_id, resolution, noise_scale, seed);
    return std::make_shared<TextureHandle>(tex_id, resolution, resolution, false, false);
}