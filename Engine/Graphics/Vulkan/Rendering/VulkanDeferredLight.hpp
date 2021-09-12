#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanDescriptors.hpp"
#include "VulkanGBufferRenderer.hpp"
#include "VulkanShadowmapping.hpp"
#include "PBR/Vulkan_BRDF_LUT.hpp"

namespace VSGE {
	class VulkanDeferredLight {
	private:
		VulkanFramebuffer* _deferred_fb;
		VulkanRenderPass* _deferred_rp;

		VulkanDescriptorPool* _deferred_pool;
		VulkanDescriptorSet* _deferred_descriptor;

		VulkanPipelineLayout* _deferred_pipeline_layout;
		VulkanPipeline* _deferred_pipeline;

		//Buffers
		VulkanBuffer* _lights_buffer;

		uint32 _fb_width;
		uint32 _fb_height;
		uint32 _camera_index;
		bool _is_envmap;
		TextureFormat _outputFormat;
	public:
		VulkanDeferredLight();
		~VulkanDeferredLight();

		void CreateFramebuffer();
		void CreateDescriptorSet();
		void CreatePipeline();

		void SetLightsBuffer(VulkanBuffer* lights_buffer);
		void SetGBuffer(VulkanGBufferRenderer* gbuffer);
		void SetGBufferFromFramebuffer(VulkanFramebuffer* fb);
		void SetShadowmapper(VulkanShadowmapping* shadowmapping);
		void SetBRDF_LUT(Vulkan_BRDF_LUT* brdf_lut);
		void SetTexture(uint32 binding, VulkanTexture* texture, VulkanSampler* sampler = nullptr);
		void SetCameraIndex(uint32 camera_index);

		VulkanFramebuffer* GetFramebuffer();
		VulkanRenderPass* GetRenderPass();
		void RecordCmdbuf(VulkanCommandBuffer* cmdbuf);
		void Resize(uint32 width, uint32 height);
		void SetEnvmap(bool envmap);
		TextureFormat GetOutputFormat();
	};
}