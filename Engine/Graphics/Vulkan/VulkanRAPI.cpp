#include "VulkanRAPI.hpp"
#include "Engine/Application.hpp"

using namespace VSGE;

VulkanRAPI* VulkanRAPI::_this = 0;

void VSGE::VulkanRAPI::Create(Window* window) {
	Application* app = Application::Get();

	Instance = new VulkanInstance;
	Instance->init(false, app->GetDescription().ApplicationName.c_str(), app->GetDescription().ApplicationVersion);

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