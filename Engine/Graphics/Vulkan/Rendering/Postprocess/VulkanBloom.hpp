#pragma once

#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanComputePipeline.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>

namespace VSGE {
	class VulkanBloom {
	private:
		VulkanShader* _bloom_calc;
		VulkanShader* _bloom_blur;
	public:
		VulkanBloom();
		~VulkanBloom();

		void Create();
		void Destroy();
	};
}