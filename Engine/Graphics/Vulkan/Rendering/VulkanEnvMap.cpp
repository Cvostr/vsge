#include "VulkanEnvMap.hpp"
#include "VulkanCamerasBuffer.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

VulkanEnvMap::VulkanEnvMap() {
	_cube_size = 256;
}
VulkanEnvMap::~VulkanEnvMap() {
	SAFE_RELEASE(_env_cube_texture);

	for (uint32 i = 0; i < 6; i++) {
		SAFE_RELEASE(_sides[i]._gbuffer);
		SAFE_RELEASE(_sides[i]._light);
	}

	SAFE_RELEASE(_envmap_cmdpool)
	SAFE_RELEASE(_envmap_gbuffers_cmdbuf)
	SAFE_RELEASE(_envmap_lights_cmdbuf)
}

void VulkanEnvMap::Create() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	for (uint32 i = 0; i < 6; i++) {
		_sides[i]._gbuffer = new VulkanGBufferRenderer;
		_sides[i]._light = new VulkanDeferredLight;

		_sides[i]._gbuffer->SetCameraIndex(ENVMAP_CAMS_POS + i);
		_sides[i]._gbuffer->CreateFramebuffer();
		_sides[i]._gbuffer->CreateDescriptorSets();
		_sides[i]._gbuffer->Resize(_cube_size, _cube_size);

		_sides[i]._light->SetCameraIndex(ENVMAP_CAMS_POS + i);
		_sides[i]._light->CreateFramebuffer();
		_sides[i]._light->CreateDescriptorSet();
		_sides[i]._light->CreatePipeline();
		_sides[i]._light->Resize(_cube_size, _cube_size);
	}

	_envmap_cmdpool = new VulkanCommandPool;
	_envmap_cmdpool->Create(device->GetGraphicsQueueFamilyIndex());

	_envmap_gbuffers_cmdbuf = new VulkanCommandBuffer;
	_envmap_gbuffers_cmdbuf->Create(_envmap_cmdpool);

	_envmap_lights_cmdbuf = new VulkanCommandBuffer;
	_envmap_lights_cmdbuf->Create(_envmap_cmdpool);

	_env_cube_texture = new VulkanTexture;
	_env_cube_texture->SetCubemap(true);
	_env_cube_texture->Create(_cube_size, _cube_size, FORMAT_RGBA, 6, 1);
}

void VulkanEnvMap::Resize(uint32 new_size) {
	_cube_size = new_size;

	for (uint32 i = 0; i < 6; i++) {
		_sides[i]._gbuffer->Resize(_cube_size, _cube_size);
		_sides[i]._light->Resize(_cube_size, _cube_size);
	}

	_env_cube_texture->Destroy();
	_env_cube_texture->Create(_cube_size, _cube_size, FORMAT_RGBA, 6, 1);
}