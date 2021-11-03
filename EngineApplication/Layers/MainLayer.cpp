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
	ResourceCache::Get()->AddResourceBundle("D:/DEV/vsge_test_assets/built/resources.map");
	Scene* scene = new Scene();
	scene->NewScene();
	VulkanRenderer::Get()->SetScene(scene);
	Application::Get()->GetLayer<SceneLayer>()->SetWorkingScene(scene);

	SceneResource* sc_res = ResourceCache::Get()->GetResource<SceneResource>("cubes_test");
	ResourceCache::Get()->GetAsyncLoader()->LoadResource(sc_res, true);

	MonoScriptingLayer::Get()->GetScriptsBlob()->LoadFromFile("D:/DEV/vsge_test_assets/built/runtime.dll");

	VSGE::SceneSerializer ss;
	ss.SetScene(scene);
	ss.DeserializeBinary(sc_res->GetLoadedData(), sc_res->GetDataDescription().size);

	Application::Get()->GetLayer<SceneLayer>()->StartScene();
}
void MainLayer::OnUpdate() {

}
void MainLayer::OnDetach() {

}