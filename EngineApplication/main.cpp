#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Physics/PhysicsLayer.hpp>
#include <Scene/SceneLayer.hpp>
#include <Resources/ResourceLayer.hpp>
#include <Resources/DefaultResources.hpp>
#include <Audio/AudioLayer.hpp>
#include <Input/InputLayer.hpp>
#include <UI/UiLayer.hpp>
#include <Graphics/Fonts.hpp>
#include <Misc/ArgsParser.hpp>
#include <Layers/MainLayer.hpp>

#include <Layers/Graphics/VulkanLayer.hpp>

#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

using namespace VSGE;

Application* VSGEMain(int argc, char* argv[]) {
	ArgsParser parser(argc, argv);

	ApplicationCreateInfo descr = { "Engine Application", 1 };
	descr.graphicsApi = GRAPHICS_API_VULKAN;
	descr.headless = parser.IsHeadless();
	descr.server = parser.IsServer();
	//descr.application_dir = parser.GetApplicationDirPath();
	descr.application_dir = "D:/DEV/vsge_test_assets/built";

	auto app = new Application(descr);
	app->GetWindow().CreateWindow(1280, 720, "Test");

	PhysicsLayer* phys_layer = new PhysicsLayer;

	MonoScriptingLayer* mono_scripting_layer = new MonoScriptingLayer;
	app->AddLayer(mono_scripting_layer);
	app->AddLayer(new SceneLayer);

	if (!descr.headless) {
		VulkanRAPI* vk = new VulkanRAPI;
		vk->Create(&app->GetWindow());
		VulkanRenderer* renderer = new VulkanRenderer;
		renderer->GetFinalPass()->SetRenderToSwapchain(true);
		renderer->GetFinalPass()->Create();
		app->SetRenderer(renderer);

		AddDefaultMaterial();
		AddDefaultMeshes();
	}

	app->AddLayer(phys_layer);

	app->AddLayer(new ResourceLayer);
	app->AddLayer(new AudioLayer);
	app->AddLayer(new InputLayer);
	

	GlyphManager* glyph_manager = new GlyphManager;
	glyph_manager->AddFontContainer("c:\\Windows\\Fonts\\Arial.ttf", "arial");

	app->AddLayer(new MainLayer);
	if(!descr.headless)
		app->AddLayer(new VulkanLayer);

	app->AddLayer(new UiLayer);

	return app;
}
