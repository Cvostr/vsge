#include "Windows_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

void Windows_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::MenuItem("Group", NULL)) {
			ImGuiLayer::Get()->RegroupWindows();
		}

		bool freezeWindows = ImGuiLayer::Get()->IsWindowsFrozen();
		ImGui::MenuItem("Freeze windows", NULL, &freezeWindows);
		ImGuiLayer::Get()->SetWindowsFreeze(freezeWindows);

		ImGui::EndMenu();
	}
}