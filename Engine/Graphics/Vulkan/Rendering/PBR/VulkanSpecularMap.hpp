#pragma once

#include "../../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

#define MAX_MAP_SIZE 256
#define MAP_FORMAT FORMAT_RGBA16F

namespace VSGE {

	struct SpecularMap_MipMapLevel {
		VkImageView _image_view;
		uint32 _resolution;
	};

	class VulkanSpecularMap {
	private:
		VulkanShader* _spmap_shader;
		VulkanComputePipeline* _spmap_pipeline;
		VulkanPipelineLayout* _spmap_pipeline_layout;

		VulkanDescriptorPool* _spmap_descr_pool;
		VulkanDescriptorSet* _spmap_descr_set;

		VulkanTexture* _envmap_input_texture;
		VulkanTexture* _spmap_output_texture;
		std::vector<SpecularMap_MipMapLevel> _mipmap_levels;

		VulkanCommandPool* _spmap_cmdpool;
		VulkanCommandBuffer* _spmap_cmdbuffer;
		VulkanSemaphore* _spmap_begin_semaphore;

		uint32 _steps_count;
		uint32 _steps_passed;
	public:

		VulkanSpecularMap();
		~VulkanSpecularMap();

		void Create();

		void Destroy();

		void FillCommandBuffer();

		void Execute(VulkanSemaphore* end_semaphore);

		void SetEnvMapInputTexture(VulkanTexture* texture);
		VulkanTexture* GetSpecularOutputTexture();
		VulkanSemaphore* GetBeginSemaphore();

		void SetStepsCount(uint32 steps_count);
	};
}