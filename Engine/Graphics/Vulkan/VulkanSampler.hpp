#pragma once

#include "../Sampler.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanSampler : public TextureSampler {
	private:
		VkSampler _sampler;
	public:
		VkSampler GetSampler();

		bool Create();

		void Destroy();

		VulkanSampler();
		VulkanSampler(const VulkanSampler&) = delete;
		~VulkanSampler();
	};
}