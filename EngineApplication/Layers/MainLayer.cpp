#include "MainLayer.hpp"
#include <Resources/ResourceCache.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Misc/ManifestParser.hpp>
#include <Resources/ResourceTypes/SceneResource.hpp>
#include <Scene/SceneSerialization.hpp>

using namespace VSGE;

MainLayer::MainLayer() {
	_scene_ready = false;
}

void MainLayer::OnAttach() {
	std::string app_dir = Application::Get()->GetDescription().application_dir;
	ResourceCache::Get()->AddResourceBundle(app_dir + "/resources.map");

	ManifestParser manifest;
	manifest.ParseFromFile(app_dir + "/app.manifest");

	SceneResource* sc_res = ResourceCache::Get()->GetResource<SceneResource>(manifest.GetMainScene());

	MonoScriptingLayer::Get()->GetScriptsBlob()->LoadFromFile(app_dir + "/runtime.dll");

	sc_res->LoadAsync(0);
	Application::Get()->GetLayer<SceneLayer>()->GetMainScene()->RunOnLoad();
}

void MainLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::SceneLoadedEvent>(event, EVENT_FUNC(OnSceneLoadedEvent));
}

void MainLayer::OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& sle) {
	_scene_ready = true;
}

void MainLayer::OnUpdate() {
	if (_scene_ready) {
		Scene* scene = SceneLayer::Get()->GetMainScene();
		if (VulkanRenderer::Get())
			VulkanRenderer::Get()->SetScene(scene);
		_scene_ready = false;
	}
}
void MainLayer::OnDetach() {

}