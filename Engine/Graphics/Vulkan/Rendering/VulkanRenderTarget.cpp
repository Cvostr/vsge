#include "VulkanRenderTarget.hpp"
#include "VulkanRenderer.hpp"

using namespace VSGE;

VulkanRenderTarget::VulkanRenderTarget():
	_output(nullptr),
	_shadowmapper(nullptr)
{
	Create();
}
VulkanRenderTarget::~VulkanRenderTarget() {
	Destroy();
}

void VulkanRenderTarget::Create() {
	VulkanRenderer* vk_renderer = VulkanRenderer::Get()->Get();

	_gbuffer_renderer = new VulkanGBufferRenderer;
	_gbuffer_renderer->CreateFramebuffer();
	_gbuffer_renderer->CreateDescriptorSets();
	_gbuffer_renderer->SetScene(vk_renderer->GetScene());
	_gbuffer_renderer->SetCameraIndex(0);

	_deferred_renderer = new VulkanDeferredLight;
	_deferred_renderer->CreateFramebuffer();
	_deferred_renderer->CreateDescriptorSet();
	_deferred_renderer->CreatePipeline();
	_deferred_renderer->SetGBuffer(_gbuffer_renderer);
	_deferred_renderer->SetCameraIndex(0);
	_deferred_renderer->UnsetIBL();

	_cmdbuf = new VulkanCommandBuffer;
	_cmdbuf->Create(vk_renderer->GetCommandPool());

	_end_semaphore = new VulkanSemaphore;
	_end_semaphore->Create();
}
void VulkanRenderTarget::Destroy() {
	SAFE_RELEASE(_end_semaphore);

	SAFE_RELEASE(_cmdbuf);

	SAFE_RELEASE(_deferred_renderer);
	SAFE_RELEASE(_gbuffer_renderer);
}
void VulkanRenderTarget::SetCameraIndex(uint32 camera_index) {
	_gbuffer_renderer->SetCameraIndex(camera_index);
	_deferred_renderer->SetCameraIndex(camera_index);
}
void VulkanRenderTarget::SetEntitiesToRender(tEntityList& entities, tEntityList& particles) {
	_gbuffer_renderer->SetEntitiesToRender(entities, particles);
}
void VulkanRenderTarget::SetBuffers(
	VulkanBuffer* transforms_buffer,
	VulkanBuffer* anims_buffer,
	VulkanBuffer* particles_buffer) 
{
	_gbuffer_renderer->SetBuffers(transforms_buffer, anims_buffer, particles_buffer);
}
void VulkanRenderTarget::SetShadowmapper(VulkanShadowmapping* shadowmapping) {
	_shadowmapper = shadowmapping;
	_deferred_renderer->SetShadowmapper(shadowmapping);
}
void VulkanRenderTarget::SetIBL(VulkanIBL* ibl) {
	_deferred_renderer->SetIBL(ibl->GetSpecularMap(), ibl->GetIrradianceMap());
}
void VulkanRenderTarget::ResizeOutput(uint32 width, uint32 height) {
	if (_width == width && _height == height)
		return;

	_width = width;
	_height = height;

	_gbuffer_renderer->Resize(width, height);
	_deferred_renderer->Resize(width, height);
	_deferred_renderer->SetGBuffer(_gbuffer_renderer);
}
void VulkanRenderTarget::SetOutput(VulkanTexture* output_texture) {
	_output = output_texture;
}
VulkanCommandBuffer* VulkanRenderTarget::GetGBufferCommandBuffer() {
	return _cmdbuf;
}
VulkanCommandBuffer* VulkanRenderTarget::GetDeferredCommandBuffer() {
	return _cmdbuf;
}
VulkanTexture* VulkanRenderTarget::GetDeferredOutput() {
	return _deferred_renderer->GetOutputTexture();
}
VulkanTexture* VulkanRenderTarget::GetGBufferNormalsAttachment() {
	return _gbuffer_renderer->GetNormalAttachment();
}
VulkanTexture* VulkanRenderTarget::GetGBufferPositionsAttachment() {
	return _gbuffer_renderer->GetPositionAttachment();
}
VulkanTexture* VulkanRenderTarget::GetGBufferDepthAttachment() {
	return _gbuffer_renderer->GetDepthAttachment();
}
VulkanSemaphore* VulkanRenderTarget::GetEndSemaphore() {
	return _end_semaphore;
}
VulkanGBufferRenderer* VulkanRenderTarget::GetGBufferRenderer() {
	return _gbuffer_renderer;
}
VulkanDeferredLight* VulkanRenderTarget::GetDeferredLightRenderer() {
	return _deferred_renderer;
}
void VulkanRenderTarget::RecordCommandBuffers() {
	VulkanRenderer* vk_renderer = VulkanRenderer::Get()->Get();
	_gbuffer_renderer->SetScene(vk_renderer->GetScene());

	_cmdbuf->Begin();
	_gbuffer_renderer->RecordCmdBuffer(_cmdbuf);
	
	if (_shadowmapper) {
		_shadowmapper->UpdateShadowrenderingDescriptors();
		_shadowmapper->RecordShadowProcessingCmdbuf(_cmdbuf);
	}

	_deferred_renderer->RecordCmdbuf(_cmdbuf);
	if (_output) {
		if (_output->IsCreated()) {
			CopyDeferredToOutput(_cmdbuf);
		}
	}

	_cmdbuf->End();
}

void VulkanRenderTarget::CopyDeferredToOutput(VulkanCommandBuffer* cmdbuf) {
	VkImageLayout old = _output->GetImageLayout();
	_output->CmdChangeLayout(cmdbuf, old, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkImageCopy copy = {};
	VkImageSubresourceLayers src = {};
	src.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	src.mipLevel = 0;
	src.baseArrayLayer = 0;
	src.layerCount = 1;

	VkImageSubresourceLayers dst = src;
	dst.baseArrayLayer = 0;

	copy.srcSubresource = src;
	copy.dstSubresource = dst;

	VkOffset3D offset;
	offset.x = 0;
	offset.y = 0;
	offset.z = 0;

	copy.srcOffset = offset;
	copy.dstOffset = offset;
	copy.extent = { _output->GetWidth(), _output->GetHeight(), 1 };

	GetDeferredOutput()->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	VkImage image = GetDeferredOutput()->GetImage();
	vkCmdCopyImage(
		cmdbuf->GetCommandBuffer(),
		image,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		_output->GetImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copy);

	GetDeferredOutput()->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	_output->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}