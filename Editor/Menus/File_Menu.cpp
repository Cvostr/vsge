#include "File_Menu.hpp"
#include <imgui.h>
#include "../Misc/ResourceImporter.hpp"
#include "../Base/PlatformSpecific.hpp"

using namespace VSGEditor;

void File_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save", NULL)) {
		}
		if (ImGui::MenuItem("Save As..", NULL)) {
		}
		if (ImGui::MenuItem("Import", NULL)) {
			std::string result = GetOpenFilePath("All supported formats\0*.dds; *.png; *fbx\0\0");
			ImportFile(result);
		}

		ImGui::EndMenu();
	}
}