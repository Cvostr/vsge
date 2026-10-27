#include "Edit_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

void Edit_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("Edit"))
	{

		if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
			
		}
		if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
			
		}

		ImGui::EndMenu();
	}
}