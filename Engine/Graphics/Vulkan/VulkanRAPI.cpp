#include "VulkanRAPI.hpp"
#include "Engine/Application.hpp"

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
	Instance->init(debugGraphics, app->GetDescription().ApplicationName.c_str(), app->GetDescription().ApplicationVersion);

	Device = CreatePrimaryDevice();

	SwapChain = new VulkanSwapChain;
	SwapChain->initSwapchain(Device);
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