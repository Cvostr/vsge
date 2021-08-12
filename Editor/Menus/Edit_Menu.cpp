#include "Edit_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

void Edit_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("Edit"))
	{

		if (ImGui::MenuItem("Undo", NULL)) {
			
		}
		if (ImGui::MenuItem("Redo", NULL)) {
			
		}

		ImGui::EndMenu();
	}
}