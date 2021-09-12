#pragma once

#include "../../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

namespace VSGE {
	class VulkanIrradianceMap {
	private:
		VulkanShader* _irmap_shader;
		VulkanComputePipeline* _irmap_pipeline;
		VulkanPipelineLayout* _irmap_pipeline_layout;

		VulkanDescriptorPool* _irmap_descr_pool;
		VulkanDescriptorSet* _irmap_descr_set;

		VulkanTexture* _irmap_input_texture;
		VulkanTexture* _irmap_output_texture;
		VulkanSampler* _envmap_sampler;

		VulkanSemaphore* _irmap_begin_semaphore;
		VulkanCommandPool* _irmap_cmdpool;
		VulkanCommandBuffer* _irmap_cmdbuffer;

		uint32 _irmap_size;
	public:

		VulkanIrradianceMap();
		~VulkanIrradianceMap();

		void Create();

		void ComputeIrmapTexture(VulkanSemaphore* end_semaphore);
		void SetInputTexture(VulkanTexture* texture);
		VulkanTexture* GetIrradianceMap();
		VulkanSemaphore* GetBeginSemaphore();
	};
}