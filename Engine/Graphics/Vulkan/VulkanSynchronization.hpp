#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanCommandBuffer.hpp"

namespace VSGE {
	class VulkanSemaphore : public IGpuObject {
	private:
		VkSemaphore _semaphore;
	public:

		VulkanSemaphore() :
			_semaphore(VK_NULL_HANDLE)
		{}


		VkSemaphore GetSemaphore() { return _semaphore; }

		bool Create();
		void Destroy();
	};

	void VulkanGraphicsSubmit(VulkanCommandBuffer& cmdbuf, VulkanSemaphore& wait, VulkanSemaphore& signal);

	void VulkanGraphicsSubmit(VulkanCommandBuffer& cmdbuf);

	void VulkanComputeSubmit(VulkanCommandBuffer& cmdbuf, VulkanSemaphore& wait, VulkanSemaphore& signal);

	void VulkanPresent(VulkanSemaphore& wait, uint32 imageIndex);

	VkResult AcquireNextImage(VulkanSemaphore& signal, uint32& imageIndex);
}