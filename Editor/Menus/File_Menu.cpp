#include "File_Menu.hpp"
#include <imgui.h>

#include <Resources/ResourceCache.hpp>
#include <Resources/ResourceTypes/MaterialResource.hpp>

#include "../Misc/ResourceImporter.hpp"
#include "../Base/PlatformSpecific.hpp"
#include "../EditorLayers/EditorLayer.hpp"

#include <Scene/SceneSerialization.hpp>

using namespace VSGEditor;
using namespace VSGE;

void SaveScene(std::string file) {
	EditorLayer* el = EditorLayer::Get();

	VSGE::SceneSerializer sc;
	sc.SetScene(el->GetScene());
	sc.Serialize(file);

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
			else
				SaveScene(el->GetOpenedSceneFile());
			//save all materials

			ResourceCache* res_cache = ResourceCache::Get();
			for (uint32 resource_i = 0; resource_i < res_cache->GetResourcesCount(); resource_i++) {
				Resource* res = res_cache->GetResources()[resource_i];
				if (res->GetResourceType() == RESOURCE_TYPE_MATERIAL) {
					MaterialResource* mat_res = (MaterialResource*)res;

					mat_res->GetMaterial()->Serialize(mat_res->GetDataDescription().file_path);
				}
			}
			
		}
		if (ImGui::MenuItem("Save As..", NULL)) {
			SaveAs();
		}
		if (ImGui::MenuItem("Import", NULL)) {
			std::string result = GetOpenFilePath("All supported formats\0*.dds; *.png; *fbx\0\0");
			ImportFile(result);
		}

		ImGui::EndMenu();
	}
}