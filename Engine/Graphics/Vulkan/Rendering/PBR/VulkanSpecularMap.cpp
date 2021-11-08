#include "VulkanSpecularMap.hpp"
#include "../VulkanRenderer.hpp"
#include "../../VulkanRAPI.hpp"

using namespace VSGE;

VulkanSpecularMap::VulkanSpecularMap() {
	_steps_count = 1;
	_steps_passed = 0;
}
VulkanSpecularMap::~VulkanSpecularMap() {
	Destroy();
}

void VulkanSpecularMap::SetStepsCount(uint32 steps_count) {
	_steps_count = steps_count;
}

void VulkanSpecularMap::Create() {
	VulkanSampler* attachment_sampler = VulkanRenderer::Get()->GetAttachmentSampler();

	_spmap_shader = new VulkanShader;
	_spmap_shader->AddShaderFromFile("spmap.comp", SHADER_STAGE_COMPUTE);

	_spmap_output_texture = new VulkanTexture;
	_spmap_output_texture->SetCubemap(true);
	_spmap_output_texture->SetStorage(true);

	uint32 mips_levels = 0;
	uint32 current_resolution = 1;
	while (current_resolution <= MAX_MAP_SIZE) {
		SpecularMap_MipMapLevel level;
		level._resolution = current_resolution;
		_mipmap_levels.push_back(level);
		current_resolution *= 2;
		mips_levels++;
	}

	_spmap_output_texture->Create(MAX_MAP_SIZE, MAX_MAP_SIZE, MAP_FORMAT, 6, mips_levels);

	for (uint32 i = 1; i < mips_levels; i++) {
		SpecularMap_MipMapLevel* mplevel = &_mipmap_levels[i];

		VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		textureImageViewInfo.image = _spmap_output_texture->GetImage();
		textureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		textureImageViewInfo.format = GetFormatVK(MAP_FORMAT);
		textureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		textureImageViewInfo.subresourceRange.baseMipLevel = i;
		textureImageViewInfo.subresourceRange.levelCount = 1;
		textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
		textureImageViewInfo.subresourceRange.layerCount = 6;
		textureImageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

		VulkanRAPI* rapi = VulkanRAPI::Get();
		VulkanDevice* device = rapi->GetDevice();

		vkCreateImageView(device->getVkDevice(), &textureImageViewInfo, nullptr, &mplevel->_image_view);
	}

	_spmap_descr_pool = new VulkanDescriptorPool;
	_spmap_descr_pool->SetDescriptorSetsCount(1);
	_spmap_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	_spmap_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, mips_levels - 1);
	_spmap_descr_pool->Create();

	_spmap_descr_set = new VulkanDescriptorSet(_spmap_descr_pool);
	_spmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_spmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, mips_levels - 1);
	_spmap_descr_set->Create();
	_spmap_descr_set->WriteDescriptorImage(0, _envmap_input_texture, attachment_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	for (uint32 i = 1; i < mips_levels; i++) {
		SpecularMap_MipMapLevel* mplevel = &_mipmap_levels[i];

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = mplevel->_image_view;
		imageInfo.sampler = nullptr;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _spmap_descr_set->GetDescriptorSet();
		descriptorWrite.dstBinding = 1;
		descriptorWrite.dstArrayElement = i - 1;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();

		vkUpdateDescriptorSets(device->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	_spmap_pipeline_layout = new VulkanPipelineLayout;
	_spmap_pipeline_layout->PushDescriptorSet(_spmap_descr_set);
	_spmap_pipeline_layout->AddPushConstantRange(0, 12, VK_SHADER_STAGE_COMPUTE_BIT);
	_spmap_pipeline_layout->Create();

	_spmap_pipeline = new VulkanComputePipeline;
	_spmap_pipeline->Create(_spmap_shader, _spmap_pipeline_layout);

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_spmap_cmdpool = new VulkanCommandPool;
	_spmap_cmdpool->Create(device->GetComputeQueueFamilyIndex());

	_spmap_cmdbuffer = new VulkanCommandBuffer();
	_spmap_cmdbuffer->Create(_spmap_cmdpool);

	_spmap_begin_semaphore = new VulkanSemaphore;
	_spmap_begin_semaphore->Create();
}

void VulkanSpecularMap::SetEnvMapInputTexture(VulkanTexture* texture) {
	_envmap_input_texture = texture;
}
VulkanTexture* VulkanSpecularMap::GetSpecularOutputTexture() {
	return _spmap_output_texture;
}
VulkanSemaphore* VulkanSpecularMap::GetBeginSemaphore() {
	return _spmap_begin_semaphore;
}
void VulkanSpecularMap::Destroy() {
	SAFE_RELEASE(_spmap_begin_semaphore)
}

void VulkanSpecularMap::FillCommandBuffer() {
	VkImageMemoryBarrier pre_irmap_barrier = GetImageBarrier(_spmap_output_texture, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_irmap_arrier = GetImageBarrier(_spmap_output_texture, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	_spmap_cmdbuffer->Begin();

	_spmap_cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {pre_irmap_barrier});
	_spmap_cmdbuffer->BindComputePipeline(*_spmap_pipeline);
	_spmap_cmdbuffer->BindDescriptorSets(*_spmap_pipeline_layout, 0, 1, _spmap_descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);

	uint32 mip_map_levels = (uint32)_mipmap_levels.size();
	const float deltaRoughness = 1.0f / std::max(float(mip_map_levels), 1.0f);

	if (_steps_passed == _steps_count)
		_steps_passed = 0;

	uint32 per_time = 6 / _steps_count;
	uint32 passed = _steps_passed * per_time;

	for (uint32_t level = 1, size = MAX_MAP_SIZE / 2; level < mip_map_levels; ++level, size /= 2) {
		const uint32_t numGroups = std::max<uint32_t>(1, size / 32);

		int32 pc_level = level - 1;
		float roughness = level * deltaRoughness;

		_spmap_cmdbuffer->PushConstants(*_spmap_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, 4, &pc_level);
		_spmap_cmdbuffer->PushConstants(*_spmap_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 4, 4, &roughness);
		_spmap_cmdbuffer->PushConstants(*_spmap_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 8, 4, &passed);


		_spmap_cmdbuffer->Dispatch(numGroups, numGroups, per_time);
	}

	_spmap_cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_irmap_arrier });

	_steps_passed++;

	//copy base mipmap level
	_spmap_output_texture->CmdChangeLayout(_spmap_cmdbuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	_envmap_input_texture->CmdChangeLayout(_spmap_cmdbuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	VkImageSubresourceLayers src = {};
	src.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	src.mipLevel = 0;
	src.baseArrayLayer = 0;
	src.layerCount = 6;

	VkImageSubresourceLayers dst = src;

	VkOffset3D offset;
	offset.x = 0;
	offset.y = 0;
	offset.z = 0;

	VkImageCopy img_copy;
	img_copy.srcSubresource = src;
	img_copy.dstSubresource = dst;
	img_copy.srcOffset = offset;
	img_copy.dstOffset = offset;
	img_copy.extent = { MAX_MAP_SIZE , MAX_MAP_SIZE , 1 };

	vkCmdCopyImage(
		_spmap_cmdbuffer->GetCommandBuffer(),
		_envmap_input_texture->GetImage(),
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		_spmap_output_texture->GetImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&img_copy);

	_spmap_output_texture->CmdChangeLayout(_spmap_cmdbuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	_envmap_input_texture->CmdChangeLayout(_spmap_cmdbuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


	_spmap_cmdbuffer->End();
}

void VulkanSpecularMap::Execute(VulkanSemaphore* end_semaphore) {
	VulkanComputeSubmit(*_spmap_cmdbuffer, *_spmap_begin_semaphore, *end_semaphore);
}