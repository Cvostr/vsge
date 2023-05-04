#include "VulkanRTRenderer.hpp"

using namespace VSGE;

VulkanRTRenderer::VulkanRTRenderer() {

}

VulkanRTRenderer::~VulkanRTRenderer() {

}

void VulkanRTRenderer::Create() {
	//Creating buffers for matrices
	m_transformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	m_transformsShaderBuffer->Create(MAX_OBJECTS_RENDER * UNI_ALIGN);

	m_animationTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	m_animationTransformsShaderBuffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	m_particlesTransformShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	m_particlesTransformShaderBuffer->Create(sizeof(Mat4) * MAX_PARTICLES_MATRICES);

	m_lightsBuffer = new LightsBuffer;
	m_lightsBuffer->Create();

	//GBUFFER
	m_gbufferRenderer = new VulkanGBufferRenderer;
	m_gbufferRenderer->CreateFramebuffer();
	m_gbufferRenderer->CreateDescriptorSets();
	m_gbufferRenderer->SetCameraIndex(0);
	m_gbufferRenderer->SetBuffers(
		m_transformsShaderBuffer, 
		m_animationTransformsShaderBuffer,
		m_particlesTransformShaderBuffer);

	//Deffered renderer
	m_deferredRenderer = new VulkanDeferredLight;
	m_deferredRenderer->CreateFramebuffer();
	m_deferredRenderer->CreateDescriptorSet(m_lightsBuffer);
	m_deferredRenderer->CreatePipeline();
	m_deferredRenderer->SetGBuffer(m_gbufferRenderer);
	m_deferredRenderer->SetCameraIndex(0);
	m_deferredRenderer->UnsetIBL();

	//Postprocessing
	m_gammaCorrection = new VulkanGammaCorrection();

	m_bloom = new VulkanBloom;
	m_bloom->Create();

	m_ssao = new VulkanSSAO;
	m_ssao->Create();

	//Shadowmapper
	/*m_shadowmapper = new VulkanShadowmapping(
		&m_gbufferRenderer->GetVertexDescriptorSets(),
		m_gbufferRenderer->GetAnimationsDescriptorSet(),
		_cameras_buffer->GetCamerasBuffer(),
		m_gbufferRenderer->GetPositionAttachment());
	m_shadowmapper->SetEntitiesToRender(&_entitiesToRender);*/
}
void VulkanRTRenderer::Destroy() {

}

void VulkanRTRenderer::ResizeOutput(uint32 width, uint32 height) {
	//resize posteffects
	m_gammaCorrection->SetInputTexture(m_deferredRenderer->GetOutputTexture());
	m_gammaCorrection->ResizeOutput(Vec2i(width, height));

	m_bloom->SetInputTextureHDR(m_deferredRenderer->GetOutputTexture());
	m_bloom->ResizeOutput(Vec2i(width, height));

	m_ssao->SetInputTextures(GetGBufferPositionsAttachment(), GetGBufferNormalsAttachment());
	m_ssao->ResizeOutput(Vec2i(width, height));
}