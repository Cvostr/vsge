#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Physics/PhysicsLayer.hpp>
#include <Scene/SceneLayer.hpp>
#include <Resources/ResourceLayer.hpp>
#include <Resources/DefaultResources.hpp>
#include <Audio/AudioLayer.hpp>
#include <Input/InputLayer.hpp>

#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

using namespace VSGE;

Application* VSGEMain() {
	ApplicationCreateInfo descr = { "Engine Application", 1 };
	descr.graphicsApi = GRAPHICS_API_VULKAN;

	auto app = new Application(descr);
	app->GetWindow().CreateWindow(1280, 720, "Test");

	VulkanRAPI* vk = new VulkanRAPI;
	vk->Create(&app->GetWindow());
	VulkanRenderer* renderer = new VulkanRenderer;

	MonoScriptingLayer* mono_scripting_layer = new MonoScriptingLayer;
	app->AddLayer(mono_scripting_layer);

	app->AddLayer(new PhysicsLayer);
	app->AddLayer(new SceneLayer);

	app->AddLayer(new ResourceLayer);
	app->AddLayer(new AudioLayer);
	app->AddLayer(new InputLayer);

	AddDefaultMaterial();
	AddDefaultMeshes();

	return app;
}
