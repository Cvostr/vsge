#include "View_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../EditorLayers/EditorLayer.hpp"

using namespace VSGEditor;

void View_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Toggle play camera", "Ctrl+Q")) {
			EditorLayer* el = EditorLayer::Get();

			EditorCameraMode mode = el->GetEditorCameraMode();

			if (mode == EDITOR_CAMERA_MODE_EDIT_CAMERA)
				mode = EDITOR_CAMERA_MODE_GAME_CAMERA;
			else if (mode == EDITOR_CAMERA_MODE_GAME_CAMERA)
				mode = EDITOR_CAMERA_MODE_EDIT_CAMERA;

			el->SetEditorCameraMode(mode);
		}

		ImGui::EndMenu();
	}
}