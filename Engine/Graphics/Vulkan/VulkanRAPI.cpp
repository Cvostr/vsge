#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanRAPI* VulkanRAPI::_this = 0;

void VSGE::VulkanRAPI::Create(Window* window) {
	Instance = new VulkanInstance;
	Instance->init(true, "XUI", 1);

	Device = CreatePrimaryDevice();


	SwapChain = new VulkanSwapChain;
	SwapChain->initSwapchain(Device,
		Instance,
		window->GetWindowWidth(), window->GetWindowHeight());
	//Create vulkan memory allocator
	Allocator = new VulkanMA(Instance, Device);
}

void VSGE::VulkanRAPI::Destroy() {
	SwapChain->Destroy();
	Device->Destroy();
	Instance->Destroy();
}