#include "VulkanIBL.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

VulkanIBL::VulkanIBL() {
	_envmap = nullptr;
	_irmap = nullptr;
	_spmap = nullptr;
	_alternately = false;
	_prev_step = 2;
	_enabled = true;
}
VulkanIBL::~VulkanIBL() {
	Destroy();
}

void VulkanIBL::SetInputData(tEntityList& entities,
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

void VulkanIBL::SetSpmapIrmapAlternately(bool alternately) {
	_alternately = alternately;
}

void VulkanIBL::SetEnabled(bool enabled) {
	_enabled = enabled;
}

void VulkanIBL::SetScene(Scene* scene) {
	_envmap->SetScene(scene);
}

void VulkanIBL::Create() {
	_envmap = new VulkanEnvMap;
	_envmap->SetInputData(
		*_entities_to_render,
		*_particles_to_render,
		_transforms_buffer,
		_animations_buffer,
		_particles_buffer,
		_lights_buffer);
	_envmap->SetStepsCount(6);
	_envmap->Create();

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_pool = new VulkanCommandPool();
	_pool->Create(device->GetGraphicsQueueFamilyIndex());

	_compute_pool = new VulkanCommandPool();
	_compute_pool->Create(device->GetComputeQueueFamilyIndex());

	_env_cmdbuf = new VulkanCommandBuffer;
	_env_cmdbuf->Create(_pool);

	_maps_cmdbuf = new VulkanCommandBuffer;
	_maps_cmdbuf->Create(_compute_pool);

	_envmap_begin_semaphore = new VulkanSemaphore;
	_envmap_begin_semaphore->Create();

	_envmap_end_semaphore = new VulkanSemaphore;
	_envmap_end_semaphore->Create();

	_irmap = new VulkanIrradianceMap;
	_irmap->Create();
	_irmap->SetStepsCount(6);
	_irmap->SetInputTexture(_envmap->GetCubeTexture());

	_spmap = new VulkanSpecularMap;
	_spmap->SetStepsCount(6);
	_spmap->SetEnvMapInputTexture(_envmap->GetCubeTexture());
	_spmap->Create();
}
void VulkanIBL::Destroy() {
	SAFE_RELEASE(_spmap)
	SAFE_RELEASE(_irmap)
	SAFE_RELEASE(_envmap)
}

void VulkanIBL::RecordCmdBufs() {
	
	_env_cmdbuf->Begin();
	if (_enabled)
		_envmap->RecordCmdbuffer(_env_cmdbuf);
	_env_cmdbuf->End();

	_maps_cmdbuf->Begin();
	if (_enabled) {
		if (_alternately) {
			if (_prev_step == 1) {
				_irmap->RecordCommandBuffer(_maps_cmdbuf);
				_prev_step = 2;
			}
			else if (_prev_step == 2) {
				_spmap->RecordCommandBuffer(_maps_cmdbuf);
				_prev_step = 1;
			}
		}
		else {
			_irmap->RecordCommandBuffer(_maps_cmdbuf);
			_spmap->RecordCommandBuffer(_maps_cmdbuf);
		}
	}
	_maps_cmdbuf->End();
}

void VulkanIBL::Execute(VulkanSemaphore* end_semaphore) {
	if (!_enabled) {
		VulkanGraphicsSubmit(*_env_cmdbuf, *_envmap_begin_semaphore, *end_semaphore);
		return;
	}

	VulkanGraphicsSubmit(*_env_cmdbuf, *_envmap_begin_semaphore, *_envmap_end_semaphore);
	VulkanComputeSubmit(*_maps_cmdbuf, *_envmap_end_semaphore, *end_semaphore);
}

VulkanSemaphore* VulkanIBL::GetBeginSemaphore() {
	return _envmap_begin_semaphore;
}

VulkanTexture* VulkanIBL::GetEnvMap() {
	return _envmap->GetCubeTexture();
}
VulkanTexture* VulkanIBL::GetIrradianceMap() {
	return _irmap->GetIrradianceMap();
}
VulkanTexture* VulkanIBL::GetSpecularMap() {
	return _spmap->GetSpecularOutputTexture();
}