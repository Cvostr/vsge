#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanCommandBuffer.hpp"

namespace VSGE {
	class VulkanSemaphore : public IGpuObject {
	private:
		VkSemaphore mSemaphore;
	public:

		VkSemaphore GetSemaphore() { return mSemaphore; }

		bool Create();
		void Destroy();
	};

	void VulkanGraphicsSubmit(VulkanCommandBuffer& cmdbuf, VulkanSemaphore& wait, VulkanSemaphore& signal);

	void VulkanGraphicsSubmit(VulkanCommandBuffer& cmdbuf);

	void VulkanPresent(VulkanSemaphore& wait, uint32 imageIndex);
}