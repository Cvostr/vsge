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

void VulkanRenderTarget::Create() 
{
	VulkanCamerasBuffer* camBuffer = VulkanRenderer::Get()->GetCamerasBuffer();

	m_gbufferRenderer = new VulkanGBufferRenderer;
	m_gbufferRenderer->CreateFramebuffer();
	m_gbufferRenderer->CreateDescriptorSets(camBuffer);
	m_gbufferRenderer->SetCameraIndex(0);

	_shadowmapper = new VulkanShadowmapping(
		&m_gbufferRenderer->GetVertexDescriptorSets(),
		m_gbufferRenderer->GetAnimationsDescriptorSet(),
		camBuffer->GetCamerasBuffer(),
		m_gbufferRenderer->GetPositionAttachment());

	_deferred_renderer = new VulkanDeferredLight;
	_deferred_renderer->CreateFramebuffer();
	_deferred_renderer->CreateDescriptorSet(VulkanRenderer::Get()->GetLightsBuffer(), camBuffer);
	_deferred_renderer->SetShadowMapDescriptorSet(_shadowmapper->GetShadowRendererDescriptorSet());
	_deferred_renderer->CreatePipeline(_shadowmapper->GetShadowRendererDescriptorSet());
	_deferred_renderer->SetGBuffer(m_gbufferRenderer);
	_deferred_renderer->SetCameraIndex(0);
	_deferred_renderer->UnsetIBL();
	_deferred_renderer->SetShadowmapper(_shadowmapper);

	_gamma_correction = new VulkanGammaCorrection();

	_bloom = new VulkanBloom;
	_bloom->Create();

	_ssao = new VulkanSSAO;
	_ssao->Create();
}
void VulkanRenderTarget::Destroy() {
	SAFE_RELEASE(_ssao)
	SAFE_RELEASE(_bloom)
	SAFE_RELEASE(_gamma_correction)
	SAFE_RELEASE(_deferred_renderer);
	SAFE_RELEASE(_shadowmapper)
	SAFE_RELEASE(m_gbufferRenderer);
}

void VulkanRenderTarget::SetCameraIndex(uint32 camera_index) 
{
	_camera_index = camera_index;
	m_gbufferRenderer->SetCameraIndex(camera_index);
	_deferred_renderer->SetCameraIndex(camera_index);

	VulkanCamerasBuffer* camBuffer = VulkanRenderer::Get()->GetCamerasBuffer();
	_shadowmapper->SetCamera(camera_index, camBuffer->GetCameraByIndex(camera_index));
}

void VulkanRenderTarget::SetEntitiesToRender(tEntityList& entities, tEntityList& particles) 
{
	m_gbufferRenderer->SetEntitiesToRender(entities, particles);
	_shadowmapper->SetEntitiesToRender(&entities);
}

void VulkanRenderTarget::SetBuffers(
	VulkanBuffer* transforms_buffer,
	VulkanBuffer* anims_buffer,
	VulkanBuffer* particles_buffer) 
{
	m_gbufferRenderer->SetBuffers(transforms_buffer, anims_buffer, particles_buffer);
}

void VulkanRenderTarget::SetGenericLightProbe(VulkanLightProbeImage* probeImage)
{
	_deferred_renderer->SetIBL((VulkanTexture*)probeImage->GetSpmap(), (VulkanTexture*) probeImage->GetIrmap());
}

void VulkanRenderTarget::ResizeOutput(uint32 width, uint32 height) 
{
	if (m_width == width && m_height == height)
		return;

	m_width = width;
	m_height = height;

	//resize gbuffer and deferred buffer
	m_gbufferRenderer->Resize(width, height);
	_deferred_renderer->Resize(width, height);
	_deferred_renderer->SetGBuffer(m_gbufferRenderer);

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
	return m_gbufferRenderer->GetNormalAttachment();
}

VulkanTexture* VulkanRenderTarget::GetGBufferPositionsAttachment() {
	return m_gbufferRenderer->GetPositionAttachment();
}

VulkanTexture* VulkanRenderTarget::GetGBufferDepthAttachment() {
	return m_gbufferRenderer->GetDepthAttachment();
}

VulkanGBufferRenderer* VulkanRenderTarget::GetGBufferRenderer() {
	return m_gbufferRenderer;
}

VulkanDeferredLight* VulkanRenderTarget::GetDeferredLightRenderer() {
	return _deferred_renderer;
}

VulkanShadowmapping* VulkanRenderTarget::GetShadowmapper()
{
	return _shadowmapper;
}

Camera* VulkanRenderTarget::GetCamera() 
{
	return VulkanRenderer::Get()->GetCamerasBuffer()->GetCameraByIndex(_camera_index);
}

void VulkanRenderTarget::RecordCommandBuffers(VulkanCommandBuffer* cmdbuffer) 
{
	Camera* cam = GetCamera();
	if (!cam)
		return;

	PostEffectsParams& params = cam->GetPostEffectParams();

	VulkanRenderer* vk_renderer = VulkanRenderer::Get()->Get();
	m_gbufferRenderer->SetScene(vk_renderer->GetScene());

	// Shadowmapping : rendering world
	_shadowmapper->ProcessShadowCasters(cmdbuffer);

	// GBuffer
	m_gbufferRenderer->RecordCmdBuffer(cmdbuffer);

	// SSAO
	if (params._ssao_enabled) {
		_ssao->SetCameraIndex(_camera_index);
		_ssao->FillCommandBuffer(cmdbuffer);
		_deferred_renderer->SetSSAO(_ssao->GetBlurredSSAO());
	}
	else
		_deferred_renderer->SetSSAO(nullptr);

	// Deferred Light
	_deferred_renderer->RecordCmdbuf(cmdbuffer);
	
	// Bloom
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