#include "VulkanSSAO.hpp"
#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>
#include <random>
#include <Math/MathBase.hpp>
#include "../VulkanRenderer.hpp"

using namespace VSGE;

#define KERNEL_COUNT 64

VulkanSSAO::VulkanSSAO() {
    _output_size = Vec2i(1280, 720);
}
VulkanSSAO::~VulkanSSAO() {
    Destroy();
}

void VulkanSSAO::CreatePrecomputedSSAO() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    //generate SSAO kernel
    std::vector<Vec4> ssaoKernel;
    for (uint32 i = 0; i < KERNEL_COUNT; ++i)
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
        ssaoKernel.push_back(Vec4(sample, 0));
    }
    //write kernel vectors to uniform buffer
    _ssao_kernel = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
    _ssao_kernel->Create(sizeof(Vec4) * ssaoKernel.size() + sizeof(Mat4) * 2); //each array member is 16 bytes
    _ssao_kernel->WriteData(0, sizeof(Vec4) * ssaoKernel.size(), ssaoKernel.data());
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

    _ssao_blur_shader = new VulkanShader;
    _ssao_blur_shader->AddShaderFromFile("ssao_blur.comp", SHADER_STAGE_COMPUTE);

    _descr_pool = new VulkanDescriptorPool;
    _descr_pool->SetDescriptorSetsCount(2);
    _descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
    _descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
    _descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3);
    _descr_pool->Create();

    _ssao_descr_set = new VulkanDescriptorSet;
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_descr_set->SetDescriptorPool(_descr_pool);
    _ssao_descr_set->Create();

    _ssao_blur_descr_set = new VulkanDescriptorSet;
    _ssao_blur_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_blur_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    _ssao_blur_descr_set->SetDescriptorPool(_descr_pool);
    _ssao_blur_descr_set->Create();

    _ssao_pp_layout = new VulkanPipelineLayout;
    _ssao_pp_layout->PushDescriptorSet(_ssao_descr_set);
    _ssao_pp_layout->Create();

    _ssao_blur_pp_layout = new VulkanPipelineLayout;
    _ssao_blur_pp_layout->PushDescriptorSet(_ssao_blur_descr_set);
    _ssao_blur_pp_layout->Create();

    _ssao_pipeline = new VulkanComputePipeline;
    _ssao_pipeline->Create(_ssao_shader, _ssao_pp_layout);

    _ssao_blur_pipeline = new VulkanComputePipeline;
    _ssao_blur_pipeline->Create(_ssao_blur_shader, _ssao_blur_pp_layout);

    _ssao_texture = new VulkanTexture;
    _ssao_texture->SetStorage(true);
    _ssao_texture->Create(1280, 720, FORMAT_RGBA);

    _ssao_blur_texture = new VulkanTexture;
    _ssao_blur_texture->SetStorage(true);
    _ssao_blur_texture->Create(1280, 720, FORMAT_RGBA);

    //output texture
    //_output = new VulkanTexture;
    //_output->SetStorage(true);
    //_output->Create(1280, 720, FORMAT_RGBA, 1, 1);

    _ssao_descr_set->WriteDescriptorImage(2, _ssao_noise,
        VulkanRenderer::Get()->GetAttachmentSampler());
    _ssao_descr_set->WriteDescriptorBuffer(3, _ssao_kernel);
    _ssao_descr_set->WriteDescriptorImage(4, _ssao_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);

    _ssao_blur_descr_set->WriteDescriptorImage(0, _ssao_texture, nullptr, 
        VK_IMAGE_LAYOUT_GENERAL);
    _ssao_blur_descr_set->WriteDescriptorImage(1, _ssao_blur_texture, nullptr,
        VK_IMAGE_LAYOUT_GENERAL);
}

void VulkanSSAO::Destroy() {

}

void VulkanSSAO::UpdateProjectionMatrix(uint32 cam_index) {
    VulkanCamerasBuffer* cb = VulkanRenderer::Get()->GetCamerasBuffer();
    Camera* cam = cb->GetCameraByIndex(0);
    Mat4 projection = cam->GetProjectionMatrix();
    Mat4 view = cam->GetViewMatrix();
    _ssao_kernel->WriteData(sizeof(Vec4) * KERNEL_COUNT, sizeof(Mat4),
        &view);
    _ssao_kernel->WriteData(sizeof(Vec4) * KERNEL_COUNT + sizeof(Mat4), sizeof(Mat4),
        &projection);
}

void VulkanSSAO::SetInputTexture(Texture* input) {
    PostprocessEffect::SetInputTexture(input);

}

void VulkanSSAO::SetInputTextures(
    Texture* input_positions,
    Texture* input_normals) {
    _ssao_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input_positions,
        VulkanRenderer::Get()->GetAttachmentSampler());
    _ssao_descr_set->WriteDescriptorImage(1, (VulkanTexture*)input_normals,
        VulkanRenderer::Get()->GetAttachmentSampler());
}

VulkanTexture* VulkanSSAO::GetBlurredSSAO() {
    return _ssao_blur_texture;
}

void VulkanSSAO::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
    UpdateProjectionMatrix();
    VkImageMemoryBarrier pre_barrier_ssao = GetImageBarrier(_ssao_texture,
        0, VK_ACCESS_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL);
    VkImageMemoryBarrier post_barrier_ssao = GetImageBarrier(_ssao_texture,
        VK_ACCESS_SHADER_WRITE_BIT, 0,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VkImageMemoryBarrier pre_barrier_blur = GetImageBarrier(_ssao_blur_texture,
        0, VK_ACCESS_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL);
    VkImageMemoryBarrier post_barrier_blur = GetImageBarrier(_ssao_blur_texture,
        VK_ACCESS_SHADER_WRITE_BIT, 0,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_barrier_ssao, pre_barrier_blur });
    cmdbuf->BindComputePipeline(*_ssao_pipeline);
    cmdbuf->BindDescriptorSets(*_ssao_pp_layout, 0, 1, _ssao_descr_set, 0,
        nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
    cmdbuf->Dispatch(_output_size.x / 16 + 1, _output_size.y / 16 + 1, 1);
    //blur SSAO texture
    cmdbuf->BindComputePipeline(*_ssao_blur_pipeline);
    cmdbuf->BindDescriptorSets(*_ssao_blur_pp_layout, 0, 1, _ssao_blur_descr_set, 0,
        nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
    cmdbuf->Dispatch(_output_size.x / 16 + 1, _output_size.y / 16 + 1, 1);

    cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_barrier_ssao, post_barrier_blur });

}

void VulkanSSAO::ResizeOutput(const Vec2i& new_size) {
    if (_output_size == new_size)
        return;

    _output_size = new_size;
    //_output->Resize(new_size.x, new_size.y);

    _ssao_texture->Resize(new_size.x, new_size.y);
    _ssao_descr_set->WriteDescriptorImage(4, _ssao_texture, 
        nullptr, VK_IMAGE_LAYOUT_GENERAL);

    _ssao_blur_texture->Resize(new_size.x, new_size.y);

    _ssao_blur_descr_set->WriteDescriptorImage(0, _ssao_texture, nullptr,
        VK_IMAGE_LAYOUT_GENERAL);
    _ssao_blur_descr_set->WriteDescriptorImage(1, _ssao_blur_texture, nullptr,
        VK_IMAGE_LAYOUT_GENERAL);
}