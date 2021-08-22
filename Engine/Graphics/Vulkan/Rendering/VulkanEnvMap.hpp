#pragma once

#include "../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

namespace VSGE {
	class VulkanEnvMap {
	private:
		VulkanTexture* _env_cube_texture;
	};
}