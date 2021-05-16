#include "File_Menu.hpp"
#include <imgui.h>
#include "../Misc/ResourceImporter.hpp"
#include "../Base/PlatformSpecific.hpp"
#include "../EditorLayers/EditorLayer.hpp"

using namespace VSGEditor;

void SaveScene(std::string file) {
	EditorLayer* el = EditorLayer::Get();
	VSGE::Serializer s;
	el->GetScene()->Serialize(s);
	s.store(file);
	el->GetOpenedSceneFile() = file;
}

void SaveAs() {
	std::string save = GetSaveFilePath("Scene file\0*.scn;");

	SaveScene(save);
}

void File_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save", NULL)) {
			EditorLayer* el = EditorLayer::Get();

			if (el->GetOpenedSceneFile().size() == 0)
				SaveAs();

			
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