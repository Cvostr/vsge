#include "VulkanIBL.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

VulkanIBL::VulkanIBL() {
	_envmap = nullptr;
	_irmap = nullptr;
	_spmap = nullptr;
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
	//_alternately = alternately;
}

void VulkanIBL::Update() {
	//_update_queued = true;
}

void VulkanIBL::SetEnvmapStepsCount(uint32 steps) {
	_envmap->SetStepsCount(steps);
}
void VulkanIBL::SetIrmapStepsCount(uint32 steps) {
	_irmap->SetStepsCount(steps);
}
void VulkanIBL::SetSpmapStepsCount(uint32 steps) {
	_spmap->SetStepsCount(steps);
}

void VulkanIBL::SetDrawEntities(bool draw) {
	_envmap->SetDrawWorld(draw);
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
	//_envmap->SetStepsCount(6);
	_envmap->Create();

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_pool = new VulkanCommandPool();
	_pool->Create(device->GetGenericQueueFamilyIndex());

	_compute_pool = new VulkanCommandPool();
	_compute_pool->Create(device->GetGenericQueueFamilyIndex());

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
	//_irmap->SetStepsCount(6);
	_irmap->SetInputTexture(_envmap->GetCubeTexture());

	_spmap = new VulkanSpecularMap;
	//_spmap->SetStepsCount(6);
	_spmap->SetEnvMapInputTexture(_envmap->GetCubeTexture());
	_spmap->Create();
}
void VulkanIBL::Destroy() {
	SAFE_RELEASE(_spmap)
	SAFE_RELEASE(_irmap)
	SAFE_RELEASE(_envmap)
}

void VulkanIBL::RecordCmdBufs(bool render)
{

	_env_cmdbuf->Begin();
	if (render)
		_envmap->RecordCmdbuffer(_env_cmdbuf);
	_env_cmdbuf->End();

	_maps_cmdbuf->Begin();
	if (render) {
		_irmap->RecordCommandBuffer(_maps_cmdbuf);
		_spmap->RecordCommandBuffer(_maps_cmdbuf);
	}
	_maps_cmdbuf->End();
}

void VulkanIBL::Execute(VulkanSemaphore* end_semaphore) {

	VulkanGraphicsSubmit(*_env_cmdbuf, *_envmap_begin_semaphore, *_envmap_end_semaphore);
	VulkanComputeSubmit(*_maps_cmdbuf, *_envmap_end_semaphore, *end_semaphore);
}

void VulkanIBL::RecordCmdBufs(VulkanLightProbeImage* destImage)
{
	_env_cmdbuf->Begin();
	_envmap->RecordCmdbuffer(_env_cmdbuf);
	_env_cmdbuf->End();

	_maps_cmdbuf->Begin();
	//if (is_update) {
	// TODO: const
		_irmap->RecordCommandBuffer(_maps_cmdbuf, destImage->GetIrmapDescrSet(), (VulkanTexture*)destImage->GetIrmap(), 32U);
		_spmap->RecordCommandBuffer(_maps_cmdbuf, destImage->GetSpmapDescrSet(), (VulkanTexture*)destImage->GetSpmap(), 256, destImage->GetSpMipmapLevelsNum());
	//}
	_maps_cmdbuf->End();
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