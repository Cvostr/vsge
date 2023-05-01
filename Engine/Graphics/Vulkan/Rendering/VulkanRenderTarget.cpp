#include "VulkanRenderTarget.hpp"
#include "VulkanRenderer.hpp"

using namespace VSGE;

VulkanRenderTarget::VulkanRenderTarget() : RenderTarget(),
	_output(nullptr),
	_shadowmapper(nullptr),
	_bloom(nullptr)
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

	_gamma_correction = new VulkanGammaCorrection();

	_bloom = new VulkanBloom;
	_bloom->Create();

	_ssao = new VulkanSSAO;
	_ssao->Create();
}
void VulkanRenderTarget::Destroy() {
	SAFE_RELEASE(_deferred_renderer);
	SAFE_RELEASE(_gbuffer_renderer);
	SAFE_RELEASE(_gamma_correction);
}
void VulkanRenderTarget::SetCameraIndex(uint32 camera_index) {
	_camera_index = camera_index;
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
	if (m_width == width && _height == height)
		return;

	m_width = width;
	_height = height;

	//resize gbuffer and deferred buffer
	_gbuffer_renderer->Resize(width, height);
	_deferred_renderer->Resize(width, height);
	_deferred_renderer->SetGBuffer(_gbuffer_renderer);
	//update shadowmapper (if exists)
	if (_shadowmapper) {
		_shadowmapper->ResizeOutput(width, height);
		_shadowmapper->SetGbufferPositionsAttachment(GetGBufferPositionsAttachment());
		_deferred_renderer->SetShadowmapper(_shadowmapper);
	}

	//resize posteffects
	_gamma_correction->SetInputTexture(_deferred_renderer->GetOutputTexture());
	_gamma_correction->ResizeOutput(Vec2i(width, height));

	_bloom->SetInputTextureHDR(_deferred_renderer->GetOutputTexture());
	_bloom->ResizeOutput(Vec2i(width, height));

	_ssao->SetInputTextures(GetGBufferPositionsAttachment(), GetGBufferNormalsAttachment());
	_ssao->ResizeOutput(Vec2i(width, height));
}
void VulkanRenderTarget::SetOutput(VulkanTexture* output_texture) {
	_output = output_texture;
}
VulkanTexture* VulkanRenderTarget::GetDeferredOutput() {
	return _deferred_renderer->GetOutputTexture();
}
VulkanTexture* VulkanRenderTarget::GetGammaCorrectedOutput() {
	return _gamma_correction->GetOutputTexture();
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
VulkanGBufferRenderer* VulkanRenderTarget::GetGBufferRenderer() {
	return _gbuffer_renderer;
}
VulkanDeferredLight* VulkanRenderTarget::GetDeferredLightRenderer() {
	return _deferred_renderer;
}

Camera* VulkanRenderTarget::GetCamera() {
	return VulkanRenderer::Get()->GetCamerasBuffer()->GetCameraByIndex(_camera_index);
}

void VulkanRenderTarget::RecordCommandBuffers(VulkanCommandBuffer* cmdbuffer) {
	Camera* cam = GetCamera();
	if (!cam)
		return;

	PostEffectsParams& params = cam->GetPostEffectParams();

	VulkanRenderer* vk_renderer = VulkanRenderer::Get()->Get();
	_gbuffer_renderer->SetScene(vk_renderer->GetScene());

	_gbuffer_renderer->RecordCmdBuffer(cmdbuffer);
	if (_shadowmapper) {
		_shadowmapper->UpdateShadowrenderingDescriptors();
		_shadowmapper->RecordShadowProcessingCmdbuf(cmdbuffer);
	}


	if (params._ssao_enabled) {
		_ssao->SetCameraIndex(_camera_index);
		_ssao->FillCommandBuffer(cmdbuffer);
		_deferred_renderer->SetSSAO(_ssao->GetBlurredSSAO());
	}
	else
		_deferred_renderer->SetSSAO(nullptr);



	_deferred_renderer->RecordCmdbuf(cmdbuffer);
	
	if (params._bloom_enabled) {
		_bloom->RecordCommandBuffer(cmdbuffer);
		_gamma_correction->SetInputBloomTexture(_bloom->GetBlurredBloomTextureHDR());
	}
	else
		_gamma_correction->SetInputBloomTexture(nullptr);



	_gamma_correction->FillCommandBuffer(cmdbuffer);

	if (_output) {
		if (_output->IsCreated()) {
			CopyDeferredToOutput(cmdbuffer);
		}
	}
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

	GetGammaCorrectedOutput()->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	VkImage image = GetGammaCorrectedOutput()->GetImage();
	vkCmdCopyImage(
		cmdbuf->GetCommandBuffer(),
		image,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		_output->GetImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copy);

	GetGammaCorrectedOutput()->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	_output->CmdChangeLayout(cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}