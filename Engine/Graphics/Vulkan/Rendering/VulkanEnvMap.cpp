#include "VulkanEnvMap.hpp"
#include "VulkanCamerasBuffer.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include "VulkanRenderer.hpp"

using namespace VSGE;

VulkanEnvMap::VulkanEnvMap() {
	_cube_size = 256;
	_steps = 1;
	_processed = 0;
	_enabled = true;
}
VulkanEnvMap::~VulkanEnvMap() {
	SAFE_RELEASE(_env_cube_texture);

	for (uint32 i = 0; i < 6; i++) {
		SAFE_RELEASE(_sides[i]._gbuffer);
		SAFE_RELEASE(_sides[i]._light);
	}
}

void VulkanEnvMap::Create() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	for (uint32 i = 0; i < 6; i++) {
		_sides[i]._gbuffer = new VulkanGBufferRenderer;
		_sides[i]._light = new VulkanDeferredLight;

		_sides[i]._gbuffer->SetCameraIndex(ENVMAP_CAMS_POS + i);
		_sides[i]._gbuffer->CreateFramebuffer();
		_sides[i]._gbuffer->CreateDescriptorSets();
		_sides[i]._gbuffer->SetBuffers(_transforms_buffer, _animations_buffer, _particles_buffer);
		_sides[i]._gbuffer->SetEntitiesToRender(*_entities_to_render, *_particles_to_render);
		_sides[i]._gbuffer->SetScene(VulkanRenderer::Get()->GetScene());
		_sides[i]._gbuffer->Resize(_cube_size, _cube_size);
		_sides[i]._gbuffer->EnableReverseCull();

		_sides[i]._light->SetCameraIndex(ENVMAP_CAMS_POS + i);
		_sides[i]._light->SetEnvmap(true);
		_sides[i]._light->CreateFramebuffer();
		_sides[i]._light->CreateDescriptorSet();
		_sides[i]._light->CreatePipeline();
		_sides[i]._light->SetLightsBuffer(_lights_buffer);
		_sides[i]._light->SetGBuffer(_sides[i]._gbuffer);
		_sides[i]._light->Resize(_cube_size, _cube_size);
	}

	_env_cube_texture = new VulkanTexture;
	_env_cube_texture->SetCubemap(true);
	_env_cube_texture->Create(_cube_size, _cube_size, _sides[0]._light->GetOutputFormat(), 6, 1);
}

void VulkanEnvMap::Resize(uint32 new_size) {
	_cube_size = new_size;

	for (uint32 i = 0; i < 6; i++) {
		_sides[i]._gbuffer->Resize(_cube_size, _cube_size);
		_sides[i]._light->Resize(_cube_size, _cube_size);
	}

	_env_cube_texture->Destroy();
	_env_cube_texture->Create(_cube_size, _cube_size, _sides[0]._light->GetOutputFormat(), 6, 1);
}

void VulkanEnvMap::RecordCmdbuffer(VulkanCommandBuffer* cmdbuf) {
	if (_processed == _steps)
		_processed = 0;

	uint32 step_c = 6 / _steps;

	if (_enabled) {
		for (uint32 i = _processed * step_c; i < _processed * step_c + step_c; i++) {
			_sides[i]._gbuffer->RecordCmdBuffer(cmdbuf);
		}

		for (uint32 i = _processed * step_c; i < _processed * step_c + step_c; i++) {
			_sides[i]._light->RecordCmdbuf(cmdbuf);
		}

		CopyImagesToCubeTexture(cmdbuf);
		_processed++;
	}
}

void VulkanEnvMap::CopyImagesToCubeTexture(VulkanCommandBuffer* cmdbuf) {
	VkImageLayout old = _env_cube_texture->GetImageLayout();
	_env_cube_texture->CmdChangeLayout(cmdbuf, old, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	uint32 step_c = 6 / _steps;

	VkImageCopy copy_args[6];
	for (uint32 i = _processed * step_c; i < _processed * step_c + step_c; i++) {
		VkImageSubresourceLayers src = {};
		src.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		src.mipLevel = 0;
		src.baseArrayLayer = 0;
		src.layerCount = 1;

		VkImageSubresourceLayers dst = src;
		dst.baseArrayLayer = i;

		copy_args[i].srcSubresource = src;
		copy_args[i].dstSubresource = dst;

		VkOffset3D offset;
		offset.x = 0;
		offset.y = 0;
		offset.z = 0;

		copy_args[i].srcOffset = offset;
		copy_args[i].dstOffset = offset;
		copy_args[i].extent = { _cube_size , _cube_size , 1};

		((VulkanTexture*)_sides[i]._light->GetFramebuffer()->GetColorAttachments()[0])->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	}

	for (uint32 i = _processed * step_c; i < _processed * step_c + step_c; i++) {
		VkImage image = ((VulkanTexture*)_sides[i]._light->GetFramebuffer()->GetColorAttachments()[0])->GetImage();
		vkCmdCopyImage(
			cmdbuf->GetCommandBuffer(),
			image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			_env_cube_texture->GetImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, 
			&copy_args[i]);

		((VulkanTexture*)_sides[i]._light->GetFramebuffer()->GetColorAttachments()[0])->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	_env_cube_texture->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VulkanTexture* VulkanEnvMap::GetCubeTexture() {
	return _env_cube_texture;
}

void VulkanEnvMap::SetInputData(
	tEntityList& entities,
	tEntityList& particles,
	VulkanBuffer* transforms,
	VulkanBuffer* animations,
	VulkanBuffer* particles_buffer,
	VulkanBuffer* lights)
{
	_entities_to_render = &entities;
	_particles_to_render = &particles;
	_transforms_buffer = transforms;
	_animations_buffer = animations;
	_particles_buffer = particles_buffer;
	_lights_buffer = lights;
}

void VulkanEnvMap::SetStepsCount(uint32 steps) {
	if (steps < 6 || 6 % steps == 0)
		_steps = steps;
}

void VulkanEnvMap::SetDrawWorld(bool draw) {
	for (int i = 0; i < 6; i++) {
		_sides[i]._light->SetDrawWorld(draw);
	}
}

void VulkanEnvMap::SetScene(Scene* scene) {
	for (uint32 i = 0; i < 6; i++) {
		_sides[i]._gbuffer->SetScene(scene);
	}
}