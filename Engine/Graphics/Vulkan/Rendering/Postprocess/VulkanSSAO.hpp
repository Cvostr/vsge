#pragma once

#include <Graphics/PostprocessEffect.hpp>
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanComputePipeline.hpp>

namespace VSGE {
	class VulkanSSAO : public PostprocessEffect {
	private:
		VulkanTexture* _ssao_noise;
		VulkanBuffer* _ssao_kernel;

		VulkanShader* _ssao_shader;
		VulkanShader* _ssao_blur_shader;

		void CreatePrecomputedSSAO();
	public:
		VulkanSSAO();
		~VulkanSSAO();

		void Create();
		void Destroy();

		void SetInputTexture(Texture* input);
		void SetInputTextures(
			Texture* input_positions, 
			Texture* input_normals);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
		VulkanTexture* GetBlurredSSAO();
	};
}