#include "VulkanIrradianceMap.hpp"
#include "../../VulkanRAPI.hpp"

using namespace VSGE;

VulkanIrradianceMap::VulkanIrradianceMap() {
    _irmap_size = 32;
    _steps_count = 1;
    _steps_passed = 0;
}
VulkanIrradianceMap::~VulkanIrradianceMap() {

}

void VulkanIrradianceMap::SetStepsCount(uint32 steps_count) {
    _steps_count = steps_count;
}

void VulkanIrradianceMap::Create() {
    VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

    _irmap_shader = new VulkanShader;
    _irmap_shader->AddShaderFromFile("irmap.comp", SHADER_STAGE_COMPUTE);

    _irmap_descr_pool = new VulkanDescriptorPool;
    _irmap_descr_pool->SetDescriptorSetsCount(1);
    _irmap_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
    _irmap_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
    _irmap_descr_pool->Create();

    _irmap_descr_set = new VulkanDescriptorSet(_irmap_descr_pool);
    _irmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    _irmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    _irmap_descr_set->Create();
    
    _irmap_pipeline_layout = new VulkanPipelineLayout;
    _irmap_pipeline_layout->PushDescriptorSet(_irmap_descr_set);
    _irmap_pipeline_layout->AddPushConstantRange(0, 4, VK_SHADER_STAGE_COMPUTE_BIT);
    _irmap_pipeline_layout->Create();

    _irmap_pipeline = new VulkanComputePipeline;
    _irmap_pipeline->Create(_irmap_shader, _irmap_pipeline_layout);

    _irmap_output_texture = new VulkanTexture;
    _irmap_output_texture->SetCubemap(true);
    _irmap_output_texture->SetStorage(true);
    _irmap_output_texture->Create(_irmap_size, _irmap_size, FORMAT_RGBA16F, 6, 1);

    _envmap_sampler = new VulkanSampler;
    _envmap_sampler->Create();

    _irmap_descr_set->WriteDescriptorImage(1, _irmap_output_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);

    _irmap_begin_semaphore = new VulkanSemaphore();
    _irmap_begin_semaphore->Create();
}

void VulkanIrradianceMap::SetInputTexture(VulkanTexture* texture) {
    _irmap_input_texture = texture;
    _irmap_descr_set->WriteDescriptorImage(0, texture, _envmap_sampler);
}

VulkanTexture* VulkanIrradianceMap::GetIrradianceMap() {
    return _irmap_output_texture;
}

VulkanSemaphore* VulkanIrradianceMap::GetBeginSemaphore() {
    return _irmap_begin_semaphore;
}

void VulkanIrradianceMap::RecordCommandBuffer(VulkanCommandBuffer* cmdbuffer) {
    VkImageMemoryBarrier pre_irmap_barrier = GetImageBarrier(_irmap_output_texture, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    VkImageMemoryBarrier post_irmap_arrier = GetImageBarrier(_irmap_output_texture, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (_steps_passed == _steps_count)
        _steps_passed = 0;

    uint32 per_time = 6 / _steps_count;
    uint32 passed = _steps_passed * per_time;

    cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_irmap_barrier });
    cmdbuffer->BindComputePipeline(*_irmap_pipeline);
    cmdbuffer->BindDescriptorSets(*_irmap_pipeline_layout, 0, 1, _irmap_descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
    
    cmdbuffer->PushConstants(*_irmap_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &passed);
    cmdbuffer->Dispatch(_irmap_size / 32, _irmap_size / 32, per_time);
    _steps_passed++;

    cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_irmap_arrier });
}