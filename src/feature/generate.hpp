#include "../rendering/renders/HeightmapEntityRenderer.h"
#include <memory>

std::shared_ptr<ModelHandle<HeightmapEntityRenderer::VertexData>> generate_model(const uint size);
void gen_noise_texture(uint texture_id, uint resolution, float noise_scale, uint32_t seed);
void gen_noise_texture(TextureHandle &texture, uint resolution, float noise_scale, uint32_t seed);
std::shared_ptr<TextureHandle> create_noise_texture(uint resolution, float noise_scale, uint32_t seed);