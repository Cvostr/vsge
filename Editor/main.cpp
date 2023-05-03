#include <Engine/Application.hpp>
#include "EditorLayers/EditorLayer.hpp"
#include "EditorLayers/ImGuiLayer.hpp"
#include <Physics/PhysicsLayer.hpp>
#include <Scene/SceneLayer.hpp>
#include <Resources/ResourceLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <MonoScripting/MonoScriptStorage.hpp>
#include <Audio/AudioLayer.hpp>
#include "EditorLayers/EditorSettingsLayer.hpp"
#include <Input/InputLayer.hpp>
#include <UI/UiLayer.hpp>
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

#include "Windows/ResourcePickWindow.hpp"
#include "Windows/StartWindow.hpp"
#include <Windows/CreateProjectWindow.hpp>
#include <Misc/VkMaterialsThumbnails.hpp>
#include <Misc/VkGizmos.hpp>

#include <Resources/DefaultResources.hpp>
#include <Graphics/FontManager.hpp>

using namespace VSGE;
using namespace VSGEditor;

Application* VSGEMain(int argc, char* argv[]) {

	ApplicationCreateInfo descr = {"Editor", 1};
	descr.graphicsApi = GRAPHICS_API_VULKAN;

	auto app = new Application(descr);

	EditorSettingsLayer* settings = new EditorSettingsLayer;
	app->AddLayer(settings);
	app->GetWindow().CreateWindow(settings->_windowWidth, settings->_windowHeight, "Editor");

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());

	MonoScriptingLayer* mono_scripting_layer = new MonoScriptingLayer;
	app->AddLayer(mono_scripting_layer);

	PhysicsLayer* phys_layer = new PhysicsLayer;

	app->AddLayer(new SceneLayer);

	VulkanRenderer* renderer = new VulkanRenderer;
	renderer->GetFinalPass()->SetRenderToSwapchain(false);
	renderer->GetFinalPass()->Create();
	renderer->GetFinalPass()->CreateFramebuffer();
	app->SetRenderer(renderer);

	app->AddLayer(new EditorLayer);
	app->AddLayer(new ImGuiLayer);
	app->AddLayer(phys_layer);
	app->AddLayer(new ResourceLayer);
	app->AddLayer(new AudioLayer);
	app->AddLayer(new InputLayer);
	app->AddLayer(new UiLayer);

	GlyphManager* glyph_manager = new GlyphManager;
	glyph_manager->AddFontContainer("c:\\Windows\\Fonts\\Arial.ttf", "arial");
	#ifdef __linux__
	glyph_manager->AddFontContainer("/usr/share/fonts/truetype/freefont/FreeSans.ttf", "arial");
	#endif


	AddDefaultMaterial();
	AddDefaultMeshes();

	VkMaterialsThumbnails* thumbs = new VkMaterialsThumbnails;
	thumbs->Create();

	VkGizmos* editor_gizmos = new VkGizmos;
	editor_gizmos->Create();

	app->GetWindow().SetPosition(settings->_windowPosX, settings->_windowPosY);

	ResourcePickerWindow* rpw = new ResourcePickerWindow;
	rpw->Hide();
	app->GetLayer<ImGuiLayer>()->AddWindow(rpw);

	app->GetLayer<ImGuiLayer>()->AddWindow(new StartWindow);

	CreateProjectWindow* cpw = new CreateProjectWindow;
	cpw->Hide();
	app->GetLayer<ImGuiLayer>()->AddWindow(cpw);

	return app;
}
