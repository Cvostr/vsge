#include "Windows_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Windows/EditorSettingsWindow.hpp"

using namespace VSGEditor;

void Windows_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("Windows"))
	{

		if (ImGui::MenuItem("Editor settings", NULL)) {
			ImGuiLayer::Get()->GetWindow<EditorSettingsWindow>()->Show();
		}

		ImGui::EndMenu();
	}
}