#include "Windows_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Windows/EditorSettingsWindow.hpp"
#include "../Windows/EnvironmentSettingsWindow.hpp"

using namespace VSGEditor;

void Windows_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("Windows"))
	{

		if (ImGui::MenuItem("Editor settings", NULL)) {
			ImGuiLayer::Get()->GetWindow<EditorSettingsWindow>()->Show();
		}
		if (ImGui::MenuItem("Environment settings", NULL)) {
			ImGuiLayer::Get()->GetWindow<EnvironmentSettingsWindow>()->Show();
		}

		ImGui::EndMenu();
	}
}