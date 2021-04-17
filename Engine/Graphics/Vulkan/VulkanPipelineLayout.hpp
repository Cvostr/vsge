#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanPipelineLayout : public IGpuObject {
	private:
		VkPipelineLayout mPLayout;

		unsigned int mPushConstantBuffersSize;
		std::vector<VkPushConstantRange> mPushConstants;
	public:

		VkPipelineLayout GetPipelineLayout() { return mPLayout; }

		bool Create();
	};
}