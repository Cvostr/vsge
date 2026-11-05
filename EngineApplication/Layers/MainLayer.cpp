#include "MainLayer.hpp"
#include <Resources/ResourceCache.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Misc/ManifestParser.hpp>
#include <Resources/ResourceTypes/SceneResource.hpp>
#include <Scene/SceneSerialization.hpp>
#include <stdexcept>

using namespace VSGE;

MainLayer::MainLayer() {
	
}

void MainLayer::OnAttach() {
	std::string app_dir = Application::Get()->GetDescription().application_dir;
	ResourceCache::Get()->AddResourceBundle(app_dir + "/resources.map");

	ManifestParser manifest;
	manifest.ParseFromFile(app_dir + "/app.manifest");

	SceneResource* sc_res = (SceneResource*)ResourceCache::Get()->GetResource(manifest.GetMainScene());
	if (sc_res == nullptr) {
		throw std::runtime_error("No main scene resource in resource cache");
	}

	MonoScriptingLayer::Get()->GetScriptsBlob()->LoadFromFile(app_dir + "/runtime.dll");

	SceneLayer::Get()->LoadSceneAsync(sc_res, 0, true);
}

void MainLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::SceneLoadedEvent>(event, EVENT_FUNC(OnSceneLoadedEvent));
}

void MainLayer::OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& sle) {
	Scene* scene = SceneLayer::Get()->GetMainScene();
	
	IRenderer* renderer = Application::Get()->GetRenderer();
	if (renderer) {
		renderer->SetScene(scene);
		renderer->QueueSkyProbeUpdate();
	}
}

void MainLayer::OnUpdate() {

}
void MainLayer::OnDetach() {

}