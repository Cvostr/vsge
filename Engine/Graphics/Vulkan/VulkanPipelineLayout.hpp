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

		VulkanPipelineLayout() :
			mPLayout(VK_NULL_HANDLE),
			mPushConstantBuffersSize(0)
		{}

		~VulkanPipelineLayout() {
			Destroy();
		}

		VkPipelineLayout GetPipelineLayout() { return mPLayout; }
		void PushDescriptorSet(VulkanDescriptorSet* set) { mSets.push_back(set); }

		bool Create();
		void Destroy();
	};
}