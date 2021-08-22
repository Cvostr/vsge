#include "VulkanIrradianceMap.hpp"

using namespace VSGE;

void VulkanIrradianceMap::Create() {
    _irmap_shader = new VulkanShader;
    _irmap_shader->AddShaderFromFile("irmap.comp", SHADER_STAGE_COMPUTE);

    _irmap_pipeline_layout = new VulkanPipelineLayout;
    //_irmap_pipeline_layout->PushDescriptorSet(_brdf_descr_set);
    _irmap_pipeline_layout->Create();

    _irmap_pipeline = new VulkanComputePipeline;
    _irmap_pipeline->Create(_irmap_shader, _irmap_pipeline_layout);

    _irmap_output_texture = new VulkanTexture;
    _irmap_output_texture->SetCubemap(true);
    _irmap_output_texture->SetStorage(true);
    _irmap_output_texture->Create(1024, 1024, FORMAT_RGBA16F, 6, 1);
    _irmap_output_texture->CreateImageView();
}

VulkanTexture* VulkanIrradianceMap::GetIrradianceMap() {
    return _irmap_output_texture;
}