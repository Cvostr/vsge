#include <Engine/Application.hpp>
#include "EditorLayers/EditorLayer.hpp"
#include "EditorLayers/ImGuiLayer.hpp"
#include <Physics/PhysicsLayer.hpp>
#include <Scene/SceneLayer.hpp>
#include <Scripting/MonoLayer.hpp>
#include <Resources/ResourceLayer.hpp>
#include <Audio/AudioLayer.hpp>
#include "EditorLayers/EditorSettingsLayer.hpp"
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

#include "Windows/ResourcePickWindow.hpp"
#include "Windows/StartWindow.hpp"

#include <Resources/DefaultResources.hpp>

using namespace VSGE;
using namespace VSGEditor;

Application* VSGEMain() {

	ApplicationCreateInfo descr = {"Editor", 1};
	descr.graphicsApi = GRAPHICS_API_VULKAN;

	auto app = new Application(descr);

	app->GetWindow().CreateWindow(1366, 768, "Editor");

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());

	VulkanRenderer* renderer = new VulkanRenderer;

	app->AddLayer(new EditorLayer);
	app->AddLayer(new ImGuiLayer);
	app->AddLayer(new PhysicsLayer);
	app->AddLayer(new SceneLayer);
	EditorSettingsLayer* settings = new EditorSettingsLayer;
	app->AddLayer(settings);
	auto mono = new MonoLayer;
	mono->SetMonoDir(settings->_mono_path);
	app->AddLayer(mono);
	app->AddLayer(new ResourceLayer);
	app->AddLayer(new AudioLayer);

	app->GetWindow().SetWindowSize(settings->_windowWidth, settings->_windowHeight);
	app->GetWindow().SetPosition(settings->_windowPosX, settings->_windowPosY);

	AddDefaultMaterial();
	AddDefaultMeshes();

	ResourcePickerWindow* rpw = new ResourcePickerWindow;
	rpw->Hide();
	app->GetLayer<ImGuiLayer>()->AddWindow(rpw);
	app->GetLayer<ImGuiLayer>()->AddWindow(new StartWindow);

	app->GetLayer<SceneLayer>()->SetWorkingScene(app->GetLayer<EditorLayer>()->GetScene());

	return app;
}
