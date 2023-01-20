#include "VulkanSynchronization.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

bool VulkanSemaphore::Create() {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device->getVkDevice(), &semaphoreInfo, nullptr, &_semaphore) != VK_SUCCESS) {
		return false;
	}

	mCreated = true;

	return true;
}
void VulkanSemaphore::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		vkDestroySemaphore(device->getVkDevice(), _semaphore, nullptr);
		mCreated = false;
	}
}

void VSGE::VulkanGraphicsSubmit(VulkanCommandBuffer& cmdbuf, VulkanSemaphore& wait, VulkanSemaphore& signal) {
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSemaphore _wait = wait.GetSemaphore();
	VkSemaphore _signal = signal.GetSemaphore();
	VkCommandBuffer cmd = cmdbuf.GetCommandBuffer();

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &_wait;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &_signal;

	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	vkQueueSubmit(device->GetGenericQueue()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->GetGenericQueue()->GetQueue());
}

void VSGE::VulkanGraphicsSubmit(VulkanCommandBuffer& cmdbuf) {
	VkCommandBuffer cmd = cmdbuf.GetCommandBuffer();

	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &cmd;

	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();

	vkQueueSubmit(device->GetGenericQueue()->GetQueue(), 1, &end_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->GetGenericQueue()->GetQueue());
}

void VSGE::VulkanComputeSubmit(VulkanCommandBuffer& cmdbuf, VulkanSemaphore& wait, VulkanSemaphore& signal) {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();
	VkCommandBuffer vcmdbuf = cmdbuf.GetCommandBuffer();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vcmdbuf;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
	VkSemaphore _wait = wait.GetSemaphore();
	VkSemaphore _signal = signal.GetSemaphore();

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &_wait;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &_signal;

	vkQueueSubmit(device->GetGenericQueue()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->GetGenericQueue()->GetQueue());
}

void VSGE::VulkanPresent(VulkanSemaphore& wait, uint32 imageIndex) {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();
	VulkanSwapChain* swapchain = vulkan->GetSwapChain();

	VkSemaphore _wait = wait.GetSemaphore();

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_wait;

	VkSwapchainKHR swapChain = swapchain->GetSwapChain();
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;

	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(device->GetGenericQueue()->GetQueue(), &presentInfo);
	vkQueueWaitIdle(device->GetGenericQueue()->GetQueue());
}

VkResult VSGE::AcquireNextImage(VulkanSemaphore& signal, uint32& imageIndex) {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	VulkanDevice* device = vulkan->GetDevice();
	VulkanSwapChain* swapchain = vulkan->GetSwapChain();

	return vkAcquireNextImageKHR(device->getVkDevice(),
		swapchain->GetSwapChain(), INT64_MAX, signal.GetSemaphore(), VK_NULL_HANDLE, &imageIndex);
}