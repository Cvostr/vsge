#pragma once

#include "VulkanGBufferRenderer.hpp"
#include "VulkanDeferredLight.hpp"
#include "Postprocess/VulkanGammaCorrection.hpp"
#include "PBR/VulkanIBL.hpp"

namespace VSGE {
	class VulkanRenderTarget {
	private:
		VulkanGBufferRenderer* _gbuffer_renderer;
		VulkanDeferredLight* _deferred_renderer;

		VulkanGammaCorrection* _gamma_correction;

		VulkanTexture* _output;

		VulkanShadowmapping* _shadowmapper;

		uint32 _width;
		uint32 _height;

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

		VulkanTexture* GetDeferredOutput();
		VulkanTexture* GetGammaCorrectedOutput();
		VulkanTexture* GetGBufferNormalsAttachment();
		VulkanTexture* GetGBufferPositionsAttachment();
		VulkanTexture* GetGBufferDepthAttachment();

		VulkanGBufferRenderer* GetGBufferRenderer();
		VulkanDeferredLight* GetDeferredLightRenderer();
	};
}