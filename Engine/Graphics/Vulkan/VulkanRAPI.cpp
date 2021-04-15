#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanRAPI* VulkanRAPI::_this = 0;

void VSGE::VulkanRAPI::Create(Window* window) {
	Instance = new Engine::ZSVulkanInstance;
	Instance->init(true, "XUI", 1, window->GetSdlWinPtr());

	Device = Engine::CreatePrimaryDevice(Instance);


	SwapChain = new Engine::ZSVulkanSwapChain;
	SwapChain->initSwapchain(Device,
		Instance,
		window->GetWindowWidth(), window->GetWindowHeight());
}