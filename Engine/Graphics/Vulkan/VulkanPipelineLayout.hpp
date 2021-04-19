#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanDescriptors.hpp"

namespace VSGE {
	class VulkanPipelineLayout : public IGpuObject {
	private:
		VkPipelineLayout mPLayout;

		unsigned int mPushConstantBuffersSize;
		std::vector<VkPushConstantRange> mPushConstants;

		std::vector<VulkanDescriptorSet*> mSets;
	public:

		VkPipelineLayout GetPipelineLayout() { return mPLayout; }
		void PushDescriptorSet(VulkanDescriptorSet* set) { mSets.push_back(set); }

		bool Create();
		void Destroy();
	};
}