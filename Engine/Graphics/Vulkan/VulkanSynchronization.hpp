#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanSemaphore : public IGpuObject {
	private:
		VkSemaphore mSemaphore;
	public:

		VkSemaphore GetSemaphore() { return mSemaphore; }

		bool Create();
		void Destroy();
	};
}