#include "VulkanGBufferRenderer.hpp"
#include "../VulkanRAPI.hpp"
#include "VulkanRenderer.hpp"

using namespace VSGE;

VulkanGBufferRenderer::VulkanGBufferRenderer() {
	_fb_width = 1280;
	_fb_height = 720;

	_camera_index = 0;
}
VulkanGBufferRenderer::~VulkanGBufferRenderer() {
	SAFE_RELEASE(_gbuffer_fb)
	SAFE_RELEASE(_gbuffer_renderpass)
}

void VulkanGBufferRenderer::CreateFramebuffer() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_gbuffer_renderpass = new VulkanRenderPass;
	_gbuffer_renderpass->SetClearSize(_fb_width, _fb_height);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushDepthAttachment(device->GetSuitableDepthFormat());
	_gbuffer_renderpass->Create();

	_gbuffer_fb = new VulkanFramebuffer;
	_gbuffer_fb->SetSize(_fb_width, _fb_height);
	_gbuffer_fb->AddAttachment(FORMAT_RGBA); //Color
	_gbuffer_fb->AddAttachment(FORMAT_RGBA16F); //Normal
	_gbuffer_fb->AddAttachment(FORMAT_RGBA16F); //Position
	_gbuffer_fb->AddAttachment(FORMAT_RGBA); //Material
	_gbuffer_fb->AddDepth(GetTextureFormat(device->GetSuitableDepthFormat()));
	_gbuffer_fb->Create(_gbuffer_renderpass);
}

void VulkanGBufferRenderer::Resize(uint32 width, uint32 height) {
	_fb_width = width;
	_fb_height = height;

	_gbuffer_renderpass->SetClearSize(width, height);
	_gbuffer_fb->Resize(_fb_width, _fb_height);
}

void VulkanGBufferRenderer::CreateDescriptorSets() {
	_objects_descr_pool = new VulkanDescriptorPool;

	//Create base vertex descriptors sets
	for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
		VulkanDescriptorSet* set = new VulkanDescriptorSet(_objects_descr_pool);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT); //Camera
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT); //Transform
		_vertex_descriptor_sets.push_back(set);
	}

	_animations_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_animations_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Animation

	//Create base particles descriptors sets
	_particles_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_particles_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);

	//Create POOL
	_objects_descr_pool->Create();

	for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
		VulkanDescriptorSet* set = _vertex_descriptor_sets[desc_i];
		set->Create();
		set->WriteDescriptorBuffer(0, VulkanRenderer::Get()->GetCamerasBuffer()->GetCamerasBuffer());
	}

	_animations_descriptor_set->Create();
	_particles_descriptor_set->Create();
}

void VulkanGBufferRenderer::SetBuffers(VulkanBuffer* transforms_buffer, VulkanBuffer* anims_buffer, VulkanBuffer* particles_buffer) {
	if (transforms_buffer) {
		_transforms_buffer = transforms_buffer;
		for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
			VulkanDescriptorSet* set = _vertex_descriptor_sets[desc_i];
			set->WriteDescriptorBuffer(1, transforms_buffer, desc_i * 1024 * 64, sizeof(Mat4) * 1024);
		}
	}
	if (anims_buffer) {
		_animations_buffer = anims_buffer;
		_animations_descriptor_set->WriteDescriptorBuffer(0, anims_buffer, 0);
	}
	if (particles_buffer) {
		_particles_buffer = particles_buffer;
		_particles_descriptor_set->WriteDescriptorBuffer(0, particles_buffer);
	}
}

void VulkanGBufferRenderer::SetEntitiesToRender(tEntityList& entities) {
	_entities_to_render = &entities;
}

void VulkanGBufferRenderer::RecordCmdBuffer(VulkanCommandBuffer* cmdbuf) {

}

void VulkanGBufferRenderer::CreateBuffers() {
	_transforms_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_transforms_buffer->Create(MAX_OBJECTS_RENDER * UNI_ALIGN);

	_animations_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_animations_buffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	_particles_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_particles_buffer->Create(sizeof(Mat4) * MAX_PARTICLES_MATRICES);
}

void VulkanGBufferRenderer::SetCameraIndex(uint32 camera_index) {
	_camera_index = camera_index;
}
VulkanFramebuffer* VulkanGBufferRenderer::GetFramebuffer() {
	return _gbuffer_fb;
}
VulkanRenderPass* VulkanGBufferRenderer::GetRenderPass() {
	return _gbuffer_renderpass;
}
VulkanTexture* VulkanGBufferRenderer::GetAlbedoAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[0];
}
VulkanTexture* VulkanGBufferRenderer::GetNormalAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[1];
}
VulkanTexture* VulkanGBufferRenderer::GetPositionAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[2];
}
VulkanTexture* VulkanGBufferRenderer::GetMaterialsAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[3];
}
VulkanTexture* VulkanGBufferRenderer::GetDepthAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetDepthAttachment();
}
VulkanBuffer* VulkanGBufferRenderer::GetTransformsBuffer() {
	return _transforms_buffer;
}
VulkanBuffer* VulkanGBufferRenderer::GetAnimationsBuffer() {
	return _animations_buffer;
}
VulkanBuffer* VulkanGBufferRenderer::GetParticlesBuffer() {
	return _particles_buffer;
}