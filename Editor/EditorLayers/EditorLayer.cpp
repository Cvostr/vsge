#include "EditorLayer.hpp" 
#include <Core/FileLoader.hpp>

#include "ImGuiLayer.hpp"

#include "../Windows/SceneWindow.hpp"
#include "../Windows/InspectorWindow.hpp"
#include "../Windows/BrowserWindow.hpp"
#include "../Windows/SceneViewWindow.hpp"

#include "../Menus/File_Menu.hpp"

using namespace VSGEditor;
using namespace VSGE;

EditorLayer* EditorLayer::_this = nullptr;

void EditorLayer::OnAttach() {

}
void EditorLayer::OnUpdate() {

}
void EditorLayer::OnDetach() {

}

void EditorLayer::OpenProjectDirectory(const std::string& dir_path) {
	ImGuiLayer::Get()->AddWindow(new SceneWindow);
	ImGuiLayer::Get()->AddWindow(new InspectorWindow);
	ImGuiLayer::Get()->AddWindow(new FileBrowserWindow(dir_path));
	ImGuiLayer::Get()->AddWindow(new SceneViewWindow);

	ImGuiLayer::Get()->AddMenu(new File_Menu);
}