#pragma once

#include <Graphics/PostprocessEffect.hpp>
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanComputePipeline.hpp>

namespace VSGE {
	class VulkanSSAO : public PostprocessEffect {
	private:
		VulkanTexture* _ssao_noise;

		VulkanShader* _ssao_shader;
		
		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _descr_set;

		VulkanComputePipeline* _ssao_pipeline;
		VulkanPipelineLayout* _ssao_pp_layout;

		void CreatePrecomputedSSAO();
	public:
		VulkanSSAO();
		~VulkanSSAO();

		void Create();
		void Destroy();
	};
}