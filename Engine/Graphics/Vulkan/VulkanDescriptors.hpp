#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanDescriptorPool : public IGpuObject {
	private:
		VkDescriptorPool Descr_Pool;
	};

	class VulkanDescriptorSet : public IGpuObject {
	private:
		VkDescriptorSet Descr_Set;
	};
}