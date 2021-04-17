#include <Engine/Application.hpp>
#include "Graphics/Vulkan/VulkanRAPI.hpp"


using namespace VSGE;

Application* VSGEMain() {
	auto app = new Application;

	app->GetWindow().CreateWindow(1280, 720, "Test", SDL_WINDOW_VULKAN);

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());

	return app;
}
