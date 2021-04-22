#pragma once

#include "../Sampler.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanSampler : public TextureSampler {
	private:
		VkSampler mSampler;
	public:
		VkSampler GetSampler() { return mSampler; }

		bool Create();

		void Destroy();

		VulkanSampler() :
			mSampler(VK_NULL_HANDLE)
		{}

		~VulkanSampler() {
			Destroy();
		}
	};
}