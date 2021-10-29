#include "VulkanIBL.hpp"

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
	_envmap->RecordCmdbufs();
	_irmap->RecordCmdBuffer();
	_spmap->FillCommandBuffer();
}

void VulkanIBL::Execute(VulkanSemaphore* end_semaphore) {
	_envmap->Execute(_spmap->GetBeginSemaphore());
	_spmap->Execute(_irmap->GetBeginSemaphore());
	_irmap->ComputeIrmapTexture(end_semaphore);
}

VulkanSemaphore* VulkanIBL::GetBeginSemaphore() {
	return _envmap->GetBeginSemaphore();
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