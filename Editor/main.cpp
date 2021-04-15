#include <Engine/Application.hpp>
#include "EditorLayers/EditorLayer.hpp"
#include "EditorLayers/ImGuiLayer.hpp"
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include "Graphics/OpenGL/OpenGL_RAPI.hpp"

#include "Windows/SceneWindow.hpp"
#include "Windows/InspectorWindow.hpp"

using namespace VSGE;
using namespace VSGEditor;

Application* VSGEMain() {
	auto app = new Application;
	
	app->GetWindow().CreateWindow(1280, 720, "Editor", SDL_WINDOW_OPENGL);

	OpenGL_RAPI* ogl = new OpenGL_RAPI;
	ogl->Create(&app->GetWindow());

	app->AddLayer(new EditorLayer);
	app->AddLayer(new ImGuiLayer);

	ImGuiLayer::Get()->AddWindow(new SceneWindow);
	ImGuiLayer::Get()->AddWindow(new InspectorWindow);

	return app;
}