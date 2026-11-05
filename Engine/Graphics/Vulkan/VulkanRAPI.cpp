#include "VulkanRAPI.hpp"
#include "Engine/Application.hpp"
#include <stdexcept>


using namespace VSGE;

VulkanRAPI* VulkanRAPI::_this = 0;

VulkanRAPI::VulkanRAPI() {
	_this = this;

	Instance = nullptr;
	Device = nullptr;
	SwapChain = nullptr;
	Allocator = nullptr;
}

void VSGE::VulkanRAPI::Create(Window* window, bool debugGraphics) 
{
	Application* app = Application::Get();

	Instance = new VulkanInstance;
	if (!Instance->init(debugGraphics, app->GetDescription().ApplicationName.c_str(), app->GetDescription().ApplicationVersion)) {
		delete Instance;
		throw std::runtime_error("Can't create Vulkan Instance");
	}

	Device = CreatePrimaryDevice(); // throws with a readable error if device creation fails

	SwapChain = new VulkanSwapChain;
	if (!SwapChain->initSwapchain(Device)) {
		delete SwapChain;
		throw std::runtime_error("Can't create Vulkan Swapchain");
	}
	//Create vulkan memory allocator
	Allocator = new VulkanMA(Instance, Device);
}

void VSGE::VulkanRAPI::Destroy() {
	SwapChain->Destroy();
	Device->Destroy();
	Instance->Destroy();
}

VulkanInstance* VulkanRAPI::GetInstance() 
{
	return Instance;
}

VulkanDevice* VulkanRAPI::GetDevice() 
{
	return Device;
}

VulkanMA* VulkanRAPI::GetAllocator() 
{
	return Allocator;
}

VulkanSwapChain* VulkanRAPI::GetSwapChain() 
{
	return SwapChain;
}