#include "MainLayer.hpp"
#include <Resources/ResourceCache.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Engine/Application.hpp>
#include <Scene/SceneLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

#include <Resources/ResourceTypes/SceneResource.hpp>
#include <Scene/SceneSerialization.hpp>

using namespace VSGE;

void MainLayer::OnAttach() {
	std::string app_dir = Application::Get()->GetDescription().application_dir;
	ResourceCache::Get()->AddResourceBundle(app_dir + "/resources.map");
	Scene* scene = new Scene();
	scene->NewScene();

	Application::Get()->GetLayer<SceneLayer>()->SetWorkingScene(scene);

	SceneResource* sc_res = ResourceCache::Get()->GetResource<SceneResource>("cubes_test");
	ResourceCache::Get()->GetAsyncLoader()->LoadResource(sc_res, true);

	MonoScriptingLayer::Get()->GetScriptsBlob()->LoadFromFile(app_dir + "/runtime.dll");

	VSGE::SceneSerializer ss;
	ss.SetScene(scene);
	ss.DeserializeBinary(sc_res->GetLoadedData(), sc_res->GetDataDescription().size);

	if (VulkanRenderer::Get())
		VulkanRenderer::Get()->SetScene(scene);

	Application::Get()->GetLayer<SceneLayer>()->StartScene();
}
void MainLayer::OnUpdate() {

}
void MainLayer::OnDetach() {

}