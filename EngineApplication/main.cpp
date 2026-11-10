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
#include <Graphics/FontManager.hpp>
#include <System/PlatformSpecific.hpp>

using namespace VSGE;

Application* VSGEMain(int argc, char* argv[]) {
	ArgsParser parser(argc, argv);

	ApplicationCreateInfo descr = { "Engine Application", 1 };
	descr.graphicsApi = GRAPHICS_API_VULKAN;
	descr.headless = parser.IsHeadless();
	descr.server = parser.IsServer();
	const auto& appDirOpt = parser.GetApplicationDirPath();
	// if argument not provided, use current workdir
	descr.application_dir = parser.GetApplicationDirPath().value_or(".");

	auto app = new Application(descr);
	app->GetWindow().CreateWindow(1280, 720, "Test");

	//WIN32_HideConsoleWindow();

	PhysicsLayer* phys_layer = new PhysicsLayer;

	MonoScriptingLayer* mono_scripting_layer = new MonoScriptingLayer(false);
	
	app->AddLayer(mono_scripting_layer);
	app->AddLayer(new SceneLayer);

	if (!descr.headless) {
		VulkanRAPI* vk = new VulkanRAPI;
		vk->Create(&app->GetWindow(), false /*parser.IsDebugGraphics()*/);
		
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
#ifdef __linux__
	glyph_manager->AddFontContainer("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", "arial");
#endif

	app->AddLayer(new MainLayer);
	if(!descr.headless)
		app->AddLayer(new VulkanLayer);

	app->AddLayer(new UiLayer);

	return app;
}
