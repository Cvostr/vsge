#pragma once

#include "VulkanGBufferRenderer.hpp"
#include "VulkanDeferredLight.hpp"
#include "PBR/VulkanIBL.hpp"

namespace VSGE {
	class VulkanRenderTarget {
	private:
		VulkanGBufferRenderer* _gbuffer_renderer;
		VulkanDeferredLight* _deferred_renderer;

		VulkanCommandBuffer* _gbuffer_cmdbuf;
		VulkanCommandBuffer* _deferred_cmdbuf;

		VulkanSemaphore* _gbuffer_end_semaphore;
		VulkanSemaphore* _deferred_end_semaphore;

		VulkanTexture* _output;

		uint32 _width;
		uint32 _height;
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

		void RecordCommandBuffers();

		VulkanTexture* GetDeferredOutput();
		VulkanTexture* GetGBufferNormalsAttachment();
		VulkanTexture* GetGBufferPositionsAttachment();
		VulkanTexture* GetGBufferDepthAttachment();

		VulkanCommandBuffer* GetGBufferCommandBuffer();
		VulkanCommandBuffer* GetDeferredCommandBuffer();

		VulkanSemaphore* GetGbufferEndSemaphore();
		VulkanSemaphore* GetDeferredEndSemaphore();

		VulkanGBufferRenderer* GetGBufferRenderer();
		VulkanDeferredLight* GetDeferredLightRenderer();
	};
}