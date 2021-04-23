#include <Engine/Application.hpp>
#include "EditorLayers/EditorLayer.hpp"
#include "EditorLayers/ImGuiLayer.hpp"
#include "Graphics/Vulkan/VulkanRAPI.hpp"

#include <System/PlatformSpecific.hpp>

using namespace VSGE;
using namespace VSGEditor;

Application* VSGEMain() {

	ApplicationDescription descr = {"Editor", 1};

	auto app = new Application(descr);

	app->GetWindow().CreateWindow(1366, 768, "Editor", SDL_WINDOW_VULKAN);

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());


	app->AddLayer(new EditorLayer);
	app->AddLayer(new ImGuiLayer);

	app->GetLayer<EditorLayer>()->OpenProjectDirectory("D:\\DEV\\VSGE\\EngineTest");

	return app;
}
