#include <Engine/Application.hpp>
#include "EditorLayers/EditorLayer.hpp"
#include "EditorLayers/ImGuiLayer.hpp"
#include "Graphics/Vulkan/VulkanRAPI.hpp"

#include "Windows/SceneWindow.hpp"
#include "Windows/InspectorWindow.hpp"
#include "Windows/BrowserWindow.hpp"

#include <System/PlatformSpecific.hpp>

using namespace VSGE;
using namespace VSGEditor;

Application* VSGEMain() {

	ApplicationDescription descr = {"Editor", 1};

	auto app = new Application(descr);

	app->GetWindow().CreateWindow(1280, 720, "Editor", SDL_WINDOW_VULKAN);

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());


	app->AddLayer(new EditorLayer);
	app->AddLayer(new ImGuiLayer);

	ImGuiLayer::Get()->AddWindow(new SceneWindow);
	ImGuiLayer::Get()->AddWindow(new InspectorWindow);
	ImGuiLayer::Get()->AddWindow(new FileBrowserWindow("C:/"));

	return app;
}
