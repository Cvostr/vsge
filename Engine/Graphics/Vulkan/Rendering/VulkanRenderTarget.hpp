#pragma once

#include "VulkanGBufferRenderer.hpp"
#include "VulkanDeferredLight.hpp"
#include "Postprocess/VulkanGammaCorrection.hpp"
#include "Postprocess/VulkanBloom.hpp"
#include "Postprocess/VulkanSSAO.hpp"
#include "PBR/VulkanIBL.hpp"
#include <Graphics/RenderTarget.hpp>

namespace VSGE {
	class VulkanRenderTarget : public RenderTarget {
	private:
		VulkanGBufferRenderer* _gbuffer_renderer;
		VulkanDeferredLight* _deferred_renderer;

		VulkanGammaCorrection* _gamma_correction;
		VulkanBloom* _bloom;
		VulkanSSAO* _ssao;

		VulkanTexture* _output;

		VulkanShadowmapping* _shadowmapper;

		void CopyDeferredToOutput(VulkanCommandBuffer* cmdbuf);
	public:
		VulkanRenderTarget();
		~VulkanRenderTarget();

		void Create();
		void Destroy();

		void SetCameraIndex(uint32 camera_index);
		void SetEntitiesToRender(tEntityList& entities, tEntityList& particles);
		void SetBuffers(
			VulkanBuffer* transforms_buffer = nullptr,
			VulkanBuffer* anims_buffer = nullptr,
			VulkanBuffer* particles_buffer = nullptr);
		void SetShadowmapper(VulkanShadowmapping* shadowmapping);
		void SetIBL(VulkanIBL* ibl);
		void ResizeOutput(uint32 width, uint32 height);
		void SetOutput(VulkanTexture* output_texture);

		void RecordCommandBuffers(VulkanCommandBuffer* cmdbuffer);

		Camera* GetCamera();

		VulkanTexture* GetDeferredOutput();
		VulkanTexture* GetGammaCorrectedOutput();
		VulkanTexture* GetGBufferNormalsAttachment();
		VulkanTexture* GetGBufferPositionsAttachment();
		VulkanTexture* GetGBufferDepthAttachment();

		VulkanGBufferRenderer* GetGBufferRenderer();
		VulkanDeferredLight* GetDeferredLightRenderer();
	};
}