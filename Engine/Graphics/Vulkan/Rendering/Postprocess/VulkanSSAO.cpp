#include "VulkanSSAO.hpp"
#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>
#include <random>
#include <Math/MathBase.hpp>
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanSSAO::VulkanSSAO() {

}
VulkanSSAO::~VulkanSSAO() {
    Destroy();
}

void VulkanSSAO::CreatePrecomputedSSAO() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    //generate SSAO kernel
    std::vector<Vec3> ssaoKernel;
    for (uint32 i = 0; i < 64; ++i)
    {
        Vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample.Normalize();
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    //write kernel vectors to uniform buffer
    _ssao_kernel = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
    _ssao_kernel->Create(sizeof(Vec3) * ssaoKernel.size());
    _ssao_kernel->WriteData(0, sizeof(Vec3) * ssaoKernel.size(), ssaoKernel.data());
    //Calculate SSAO noise vectors
    std::vector<Vec4> ssaoNoise;
    for (uint32 i = 0; i < 16; i++)
    {
        Vec4 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f, 0.0f);
        ssaoNoise.push_back(noise);
    }
    //write SSAO noise vectors to texture
    _ssao_noise = new VulkanTexture;
    _ssao_noise->Create(4, 4, FORMAT_RGBA32F, 1, 1);
    _ssao_noise->AddMipLevel((byte*)ssaoNoise.data(),
        ssaoNoise.size() * sizeof(Vec4),
        4, 4, 0, 0);
    _ssao_noise->SetReadyToUseInShaders();
}
void VulkanSSAO::Create() {
    CreatePrecomputedSSAO();

    _ssao_shader = new VulkanShader;
    _ssao_shader->AddShaderFromFile("ssao.comp", SHADER_STAGE_COMPUTE);

    _descr_pool = new VulkanDescriptorPool;
    _descr_pool->SetDescriptorSetsCount(2);
    _descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
    _descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
    _descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
    _descr_pool->Create();

    _ssao_descr_set = new VulkanDescriptorSet;
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->SetDescriptorPool(_descr_pool);
    _ssao_descr_set->Create();

    _ssao_pp_layout = new VulkanPipelineLayout;
    _ssao_pp_layout->PushDescriptorSet(_ssao_descr_set);
    _ssao_pp_layout->Create();

    _ssao_pipeline = new VulkanComputePipeline;
    _ssao_pipeline->Create(_ssao_shader, _ssao_pp_layout);
}
void VulkanSSAO::Destroy() {

}

void VulkanSSAO::SetInputTexture(Texture* input) {
    PostprocessEffect::SetInputTexture(input);
}

void VulkanSSAO::SetInputTextures(Texture* input_positions, Texture* input_normals) {
    _ssao_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input_positions,
        VulkanRenderer::Get()->GetAttachmentSampler());
    _ssao_descr_set->WriteDescriptorImage(1, (VulkanTexture*)input_normals,
        VulkanRenderer::Get()->GetAttachmentSampler());
}

void VulkanSSAO::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {

}
void VulkanSSAO::ResizeOutput(const Vec2i& new_size) {
    if (_output_size == new_size)
        return;

    _output_size = new_size;
    _output->Resize(new_size.x, new_size.y);
}