#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanDescriptors.hpp"
#include "VulkanGBufferRenderer.hpp"
#include "VulkanShadowmapping.hpp"

namespace VSGE {
	class VulkanDefferedLight {
	private:
		VulkanFramebuffer* _deferred_fb;
		VulkanRenderPass* _deferred_rp;

		VulkanDescriptorPool* _deferred_pool;
		VulkanDescriptorSet* _deferred_descriptor;

		VulkanPipelineLayout* _deferred_pipeline_layout;
		VulkanPipeline* _deferred_pipeline;

		//Buffers
		VulkanBuffer* _cam_buffer;
		VulkanBuffer* _lights_buffer;

		uint32 _fb_width;
		uint32 _fb_height;
	public:
		VulkanDefferedLight();
		~VulkanDefferedLight();

		void CreateFramebuffer();
		void CreateDescriptorSet();
		void CreatePipeline();

		void SetCamerasBuffer(VulkanBuffer* cam_buffer);
		void SetLightsBuffer(VulkanBuffer* lights_buffer);
		void SetGBuffer(VulkanGBufferRenderer* gbuffer);
		void SetShadowmapper(VulkanShadowmapping* shadowmapping);

		VulkanFramebuffer* GetFramebuffer();
		VulkanRenderPass* GetRenderPass();
		void RecordCmdbuf(VulkanCommandBuffer* cmdbuf);
	};
}