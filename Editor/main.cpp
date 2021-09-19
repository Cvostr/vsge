#include <Engine/Application.hpp>
#include "EditorLayers/EditorLayer.hpp"
#include "EditorLayers/ImGuiLayer.hpp"
#include <Physics/PhysicsLayer.hpp>
#include <Scene/SceneLayer.hpp>
#include <Resources/ResourceLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Audio/AudioLayer.hpp>
#include "EditorLayers/EditorSettingsLayer.hpp"
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

#include "Windows/ResourcePickWindow.hpp"
#include "Windows/StartWindow.hpp"
#include <Misc/VkMaterialsThumbnails.hpp>

#include <Resources/DefaultResources.hpp>

using namespace VSGE;
using namespace VSGEditor;

Application* VSGEMain() {

	ApplicationCreateInfo descr = {"Editor", 1};
	descr.graphicsApi = GRAPHICS_API_VULKAN;

	auto app = new Application(descr);

	EditorSettingsLayer* settings = new EditorSettingsLayer;
	app->AddLayer(settings);
	app->GetWindow().CreateWindow(settings->_windowWidth, settings->_windowHeight, "Editor");

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());

	MonoScriptingLayer* mono_scripting_layer = new MonoScriptingLayer;
	mono_scripting_layer->CreateDomain();
	app->AddLayer(mono_scripting_layer);

	VulkanRenderer* renderer = new VulkanRenderer;

	app->AddLayer(new EditorLayer);
	app->AddLayer(new ImGuiLayer);
	app->AddLayer(new PhysicsLayer);
	app->AddLayer(new SceneLayer);
	
	

	app->AddLayer(new ResourceLayer);
	app->AddLayer(new AudioLayer);

	AddDefaultMaterial();
	AddDefaultMeshes();
	VkMaterialsThumbnails* thumbs = new VkMaterialsThumbnails;
	thumbs->Create();

	app->GetWindow().SetPosition(settings->_windowPosX, settings->_windowPosY);



	ResourcePickerWindow* rpw = new ResourcePickerWindow;
	rpw->Hide();
	app->GetLayer<ImGuiLayer>()->AddWindow(rpw);
	app->GetLayer<ImGuiLayer>()->AddWindow(new StartWindow);

	app->GetLayer<SceneLayer>()->SetWorkingScene(app->GetLayer<EditorLayer>()->GetScene());

	return app;
}
