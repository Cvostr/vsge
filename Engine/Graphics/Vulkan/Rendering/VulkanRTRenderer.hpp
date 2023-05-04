#pragma once

#include "VulkanGBufferRenderer.hpp"
#include "VulkanDeferredLight.hpp"
#include "Postprocess/VulkanGammaCorrection.hpp"
#include "Postprocess/VulkanBloom.hpp"
#include "Postprocess/VulkanSSAO.hpp"
#include "PBR/VulkanIBL.hpp"
#include <Graphics/RenderTarget.hpp>
#include <Graphics/LightsBuffer.hpp>

namespace VSGE {
	class VulkanRTRenderer {
	private:
		VulkanShadowmapping* m_shadowmapper;
		VulkanGBufferRenderer* m_gbufferRenderer;
		VulkanDeferredLight* m_deferredRenderer;

		VulkanGammaCorrection* m_gammaCorrection;
		VulkanBloom* m_bloom;
		VulkanSSAO* m_ssao;

		LightsBuffer* m_lightsBuffer;
		VulkanBuffer* m_transformsShaderBuffer;
		VulkanBuffer* m_animationTransformsShaderBuffer;
		VulkanBuffer* m_particlesTransformShaderBuffer;
	public:
		VulkanRTRenderer();
		~VulkanRTRenderer();

		void Create();
		void Destroy();

		void ResizeOutput(uint32 width, uint32 height);

		void RecordCommandBuffers(VulkanCommandBuffer* cmdbuffer);

		VulkanTexture* GetDeferredOutput();
		VulkanTexture* GetGammaCorrectedOutput();
		VulkanTexture* GetGBufferNormalsAttachment();
		VulkanTexture* GetGBufferPositionsAttachment();
		VulkanTexture* GetGBufferDepthAttachment();

		VulkanGBufferRenderer* GetGBufferRenderer();
		VulkanDeferredLight* GetDeferredLightRenderer();
	};
}