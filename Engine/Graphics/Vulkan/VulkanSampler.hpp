#pragma once

#include "../Sampler.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanSampler : public TextureSampler {
	private:
		VkSampler _sampler;
	public:
		VkSampler GetSampler() { return _sampler; }

		bool Create();

		void Destroy();

		VulkanSampler() :
			_sampler(VK_NULL_HANDLE)
		{}

		~VulkanSampler() {
			Destroy();
		}
	};
}