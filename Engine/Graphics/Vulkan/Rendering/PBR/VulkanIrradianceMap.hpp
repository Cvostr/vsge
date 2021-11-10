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

		uint32 _irmap_size;
		uint32 _steps_count;
		uint32 _steps_passed;
	public:

		VulkanIrradianceMap();
		~VulkanIrradianceMap();

		void Create();
		void RecordCommandBuffer(VulkanCommandBuffer* cmdbuffer);
		void SetInputTexture(VulkanTexture* texture);
		VulkanTexture* GetIrradianceMap();
		VulkanSemaphore* GetBeginSemaphore();
		void SetStepsCount(uint32 steps_count);
	};
}