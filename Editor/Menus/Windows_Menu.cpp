#include "Windows_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Windows/EditorSettingsWindow.hpp"
#include "../Windows/EnvironmentSettingsWindow.hpp"
#include "../Windows/ProjectSettingsWindow.hpp"

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
		if (ImGui::MenuItem("Project settings", NULL)) {
			ImGuiLayer::Get()->GetWindow<ProjectSettingsWindow>()->SetDrawMode(MODE_PROJECT);
			ImGuiLayer::Get()->GetWindow<ProjectSettingsWindow>()->Show();
		}
		if (ImGui::MenuItem("Application settings", NULL)) {
			ImGuiLayer::Get()->GetWindow<ProjectSettingsWindow>()->SetDrawMode(MODE_APP);
			ImGuiLayer::Get()->GetWindow<ProjectSettingsWindow>()->Show();
		}

		ImGui::EndMenu();
	}
}