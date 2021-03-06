#include "File_Menu.hpp"
#include <imgui.h>

#include <Resources/ResourceCache.hpp>
#include <Resources/ResourceTypes/MaterialResource.hpp>
#include <Engine/Application.hpp>
#include <Misc/DialogWindows.hpp>
#include "../Misc/ResourceImporter.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include <Scene/SceneLayer.hpp>

#include <Scene/SceneSerialization.hpp>
#include "../EditorLayers/ImGuiLayer.hpp"
#include <Windows/ProjectBuildingWindow.hpp>
#include <Misc/DialogWindows.hpp>

using namespace VSGEditor;
using namespace VSGE;

void SaveScene(std::string file) {
	EditorLayer* el = EditorLayer::Get();

	VSGE::SceneSerializer sc;
	sc.SetScene(SceneLayer::Get()->GetMainScene());
	sc.Serialize(file);

	el->GetOpenedSceneFile() = file;
}

void SaveAs() {
	FileDialogDesc desc;
	desc.dialog_title = "test";
	desc.extensions = {{"Scene", "*.scn"}};
	std::string save;
	SaveFileDialog(&desc, save);

	SaveScene(save);
}

void File_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save", NULL)) {
			OnSave();
		}
		if (ImGui::MenuItem("Save As..", NULL)) {
			SaveAs();
		}
		if (ImGui::MenuItem("Import", NULL)) {
			FileDialogDesc desc;
			desc.dialog_title = "Import resource";
			desc.extensions = {{"DDS Texture", "*.dds"}, {"PNG Texture", "*.png"}, {"Autodesk FBX 3D Model", "*.fbx"}};
			std::string path;
			OpenFileDialog(&desc, path);

			ImportFile(path);
		}
		if (ImGui::MenuItem("Build", NULL)) {
			ImGuiLayer::Get()->GetWindow<ProjectBuildingWindow>()->Show();
		}

		ImGui::EndMenu();
	}
}

void File_Menu::OnSave() {
	EditorLayer* el = EditorLayer::Get();
	VSGE::Application* app = VSGE::Application::Get();
	VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

	if (!scene_layer->IsSceneRunning()) {

		if (el->GetOpenedSceneFile().size() == 0)
			SaveAs();
		else
			SaveScene(el->GetOpenedSceneFile());
		//save all materials

		ResourceCache* res_cache = ResourceCache::Get();
		for (uint32 resource_i = 0; resource_i < res_cache->GetResourcesCount(); resource_i++) {
			Resource* res = res_cache->GetResources()[resource_i];
			if (res->GetResourceType() == RESOURCE_TYPE_MATERIAL && !res->IsDefault()) {
				MaterialResource* mat_res = (MaterialResource*)res;

				std::string dest_path = mat_res->GetDataDescription().file_path;
				if (dest_path.size() > 0 && mat_res->GetState() == RESOURCE_STATE_READY)
					mat_res->GetMaterial()->Serialize(dest_path);
			}
		}
	}
	else {
		MessageDialogDesc desc;
		desc.dialog_title = "Error saving scene!";
		desc.message = "Unable to save scene in playmode";
		desc.dialog_type = MessageDialogType::MESSAGE_DIALOG_TYPE_ERROR;
		DialogUserAction action;
		MessageDialog(&desc, action);
	}

}