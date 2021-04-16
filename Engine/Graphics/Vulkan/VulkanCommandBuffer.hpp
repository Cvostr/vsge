#pragma once

#include <vulkan/vulkan.hpp>

namespace VSGE {
	VkCommandPool beginCommandPool();
	VkCommandBuffer CreateSingleTimeComdbuf(VkCommandPool commandPool);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool);
}