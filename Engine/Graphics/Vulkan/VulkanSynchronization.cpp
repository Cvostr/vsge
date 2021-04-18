#include "VulkanSynchronization.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

bool VulkanSemaphore::Create() {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device->getVkDevice(), &semaphoreInfo, nullptr, &mSemaphore) != VK_SUCCESS) {
		return false;
	}

	mCreated = true;

	return true;
}
void VulkanSemaphore::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		vkDestroySemaphore(device->getVkDevice(), mSemaphore, nullptr);
		mCreated = false;
	}
}