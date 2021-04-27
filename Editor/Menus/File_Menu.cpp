#include "File_Menu.hpp"
#include <imgui.h>

using namespace VSGEditor;

void File_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save", NULL)) {
		}
		if (ImGui::MenuItem("Save As..", NULL)) {
		}
		if (ImGui::MenuItem("Import", NULL)) {
		}

		ImGui::EndMenu();
	}
}