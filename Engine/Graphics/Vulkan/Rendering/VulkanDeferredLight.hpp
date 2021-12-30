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

		VulkanGBufferRenderer* _gbuffer;

		uint32 _fb_width;
		uint32 _fb_height;
		uint32 _camera_index;
		bool _is_envmap;

		void DrawSkybox(VulkanCommandBuffer* cmdbuf);
		void DrawParticles(VulkanCommandBuffer* cmdbuf);
		void DrawEntities(VulkanCommandBuffer* cmdbuf);
	public:
		VulkanDeferredLight();
		~VulkanDeferredLight();

		void CreateFramebuffer();
		void CreateDescriptorSet();
		void CreatePipeline();

		void SetLightsBuffer(VulkanBuffer* lights_buffer);
		void SetGBuffer(VulkanGBufferRenderer* gbuffer);
		void SetShadowmapper(VulkanShadowmapping* shadowmapping);
		void SetTexture(uint32 binding, VulkanTexture* texture, VulkanSampler* sampler = nullptr);
		void SetCameraIndex(uint32 camera_index);
		void UnsetIBL();
		void SetIBL(VulkanTexture* specular, VulkanTexture* irradiance);

		VulkanFramebuffer* GetFramebuffer();
		VulkanRenderPass* GetRenderPass();
		VulkanTexture* GetOutputTexture();
		VulkanDescriptorSet* GetDeferredDescriptorSet();
		void RecordCmdbuf(VulkanCommandBuffer* cmdbuf);
		void Resize(uint32 width, uint32 height);
		void SetEnvmap(bool envmap);
		TextureFormat GetOutputFormat();
	};
}