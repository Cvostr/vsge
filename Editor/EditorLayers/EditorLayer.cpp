#include "EditorLayer.hpp" 
#include <Core/FileLoader.hpp>
#include <Engine/Application.hpp>
#include <Core/Time.hpp>
#include <filesystem>
#include "ImGuiLayer.hpp"

#include "../Windows/SceneWindow.hpp"
#include "../Windows/InspectorWindow.hpp"
#include "../Windows/BrowserWindow.hpp"
#include "../Windows/SceneViewWindow.hpp"
#include "../Windows/ToolbarWindow.hpp"
#include "../Windows/ConsoleWindow.hpp"
#include "../Windows/EditorSettingsWindow.hpp"
#include "../Windows/EnvironmentSettingsWindow.hpp"
#include "../Windows/ProjectSettingsWindow.hpp"
#include <Misc/DialogWindows.hpp>

#include <InspectorInterfaces/EntityComponents/EntityComponents.hpp>

#include "../Menus/File_Menu.hpp"
#include "../Menus/Edit_Menu.hpp"
#include "../Menus/Windows_Menu.hpp"

#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

#include <Math/Ray.hpp>
#include <ImGuizmo.h>

#define MONEMENT_COEFF 40.3f

using namespace VSGEditor;
using namespace VSGE;
namespace fs = std::filesystem;

EditorLayer* EditorLayer::_this = nullptr;

void EditorLayer::OnAttach() {

}
void EditorLayer::OnUpdate() {

}
void EditorLayer::OnDetach() {

}

bool EditorLayer::OpenProjectDirectory(const std::string& dir_path) {
	if (!fs::is_directory(dir_path)) {
		MessageDialogDesc desc;
		desc.dialog_title = "Error opening project";
		desc.message = "Directory " + dir_path + " doesn't exist!";
		desc.buttons = MESSAGE_DIALOG_BTN_OK;
		desc.dialog_type = MESSAGE_DIALOG_TYPE_ERROR;
		DialogUserAction action;
		MessageDialog(&desc, action);
		if (action == DIALOG_USER_ACTION_ACCEPT) {
			return true;
		}
	}

	mResourcesWatcher->WatchDirectory(dir_path);
	ResourceCache::Get()->AddResourceDir(dir_path);
	

	ImGuiLayer::Get()->AddWindow(new SceneWindow);
	ImGuiLayer::Get()->AddWindow(new InspectorWindow);
	ImGuiLayer::Get()->AddWindow(new ConsoleWindow);
	ImGuiLayer::Get()->AddWindow(new FileBrowserWindow(dir_path));
	ImGuiLayer::Get()->AddWindow(new SceneViewWindow);
	ImGuiLayer::Get()->AddWindow(new ToolbarWindow);

	EditorSettingsWindow* settings = new EditorSettingsWindow;
	settings->Hide();
	ImGuiLayer::Get()->AddWindow(settings);

	EnvironmentSettingsWindow* env_settings = new EnvironmentSettingsWindow;
	env_settings->Hide();
	ImGuiLayer::Get()->AddWindow(env_settings);

	ProjectSettingsWindow* proj_settings = new ProjectSettingsWindow;
	proj_settings->Hide();
	ImGuiLayer::Get()->AddWindow(proj_settings);

	ImGuiLayer::Get()->AddMenu(new File_Menu);
	ImGuiLayer::Get()->AddMenu(new Edit_Menu);
	ImGuiLayer::Get()->AddMenu(new Windows_Menu);

	VulkanRenderer::Get()->SetScene(this->mScene);

	return false;
}

void EditorLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::EventMouseMotion>(event, EVENT_FUNC(EditorLayer::OnMouseMotion));
	DispatchEvent<VSGE::EventWindowClose>(event, EVENT_FUNC(EditorLayer::OnWindowClose));
	DispatchEvent<VSGE::EventMouseScrolled>(event, EVENT_FUNC(EditorLayer::OnMouseScroll));
	DispatchEvent<VSGE::EventMouseButtonDown>(event, EVENT_FUNC(EditorLayer::OnMouseButtonDown));
	DispatchEvent<VSGE::EventMouseButtonUp>(event, EVENT_FUNC(EditorLayer::OnMouseButtonUp));
	DispatchEvent<VSGE::EventKeyButtonUp>(event, EVENT_FUNC(EditorLayer::OnKeyUp));
	DispatchEvent<VSGE::EventKeyButtonDown>(event, EVENT_FUNC(EditorLayer::OnKeyDown));
	DispatchEvent<VSGE::FileChageEvent>(event, EVENT_FUNC(EditorLayer::OnFileEvent));
	DispatchEvent<VSGE::MessageEvent>(event, EVENT_FUNC(EditorLayer::OnMessageEvent));
}

void EditorLayer::OnMouseMotion(const VSGE::EventMouseMotion& motion) {
	InputState.cursorx = motion.GetMouseX();
	InputState.cursory = motion.GetMouseY();

	ImGuiLayer* imgui = ImGuiLayer::Get();
	SceneViewWindow* win = imgui->GetWindow<SceneViewWindow>();
	if (win) {
		if (win->IsInFocus() && win->isInsideWindow(motion.GetMouseX(), motion.GetMouseY()) && InputState.right_btn_hold) {

			int relX = motion.GetMouseRelativeX();
			int relY = motion.GetMouseRelativeY();

			CameraState.cam_yaw += relX * 0.16f;
			CameraState.cam_pitch += relY * 0.16f;

			if (CameraState.cam_pitch > 89.0f)
				CameraState.cam_pitch = 89.0f;
			if (CameraState.cam_pitch < -89.0f)
				CameraState.cam_pitch = -89.0f;

			Vec3 front;
			front.x = static_cast<float>(cos(to_radians(CameraState.cam_yaw)) * cos(to_radians(CameraState.cam_pitch)));
			front.y = -sin(to_radians(CameraState.cam_pitch));
			front.z = sin(to_radians(CameraState.cam_yaw)) * cos(to_radians(CameraState.cam_pitch));
			mEditorCamera->SetFront(front.GetNormalized());
		}
	}
}

void EditorLayer::OnMouseScroll(const VSGE::EventMouseScrolled& scroll) {
	ImGuiLayer* imgui = ImGuiLayer::Get();
	SceneViewWindow* win = imgui->GetWindow<SceneViewWindow>();
	if (win) {
		if (win->IsInFocus() && win->isInsideWindow(InputState.cursorx, InputState.cursory)) {
			Vec3 cam_front = mEditorCamera->GetFront() * (float)scroll.GetOffsetY();
			Vec3 new_pos = mEditorCamera->GetPosition() + cam_front;
			mEditorCamera->SetPosition(new_pos);
		}
	}
}

void EditorLayer::OnMouseButtonDown(const VSGE::EventMouseButtonDown& mbd) {

	if (mbd.GetMouseButton() == MOUSE_BUTTON_RIGHT) {
		InputState.right_btn_hold = true;
	}
	if (mbd.GetMouseButton() == MOUSE_BUTTON_LEFT) {
		InputState.left_btn_hold = true;

		//Try to pick object
		ImGuiLayer* imgui = ImGuiLayer::Get();
		SceneViewWindow* win = imgui->GetWindow<SceneViewWindow>();
		InspectorWindow* insp = imgui->GetWindow<InspectorWindow>();
		if (win) {
			if (!win->IsTransformGizmoUsed() && 
				win->IsInFocus() && 
				win->isInsideWindow(InputState.cursorx, InputState.cursory)) {

				int relx = InputState.cursorx - win->GetPos().x;
				int rely = -InputState.cursory + win->GetPos().y + win->GetSize().y;

				bool picking_allowed = true;
				if (_pickedEntity) {
					Vec3 mpos = _pickedEntity->GetWorldTransform().GetPosition();
					ImGuizmo::vec_t model_pos = ImGuizmo::makeVect(mpos.x, mpos.y, mpos.z);
					Mat4 _cam_transform = mEditorCamera->GetProjectionViewMatrix();
					ImGuizmo::matrix_t cam_transform;
					memcpy(&cam_transform, &_cam_transform, 64);
					ImVec2 point_pos = ImGuizmo::worldToPos(model_pos, cam_transform);
					point_pos.x -= win->GetPos().x;
					point_pos.y -= win->GetPos().y;

					if (abs(point_pos.x - relx) < 75 && abs(point_pos.y - rely) < 75)
						return;
				}

				Vec2 crpos = Vec2((float)relx / win->GetSize().x,
					(float)rely / win->GetSize().y
				);

				Vec3 world_pos = mEditorCamera->ScreenPointToWorldPoint(crpos);
				Vec3 dir = world_pos - mEditorCamera->GetPosition();

				VulkanRenderer* renderer = VulkanRenderer::Get();

				Ray ray(mEditorCamera->GetPosition(), dir.GetNormalized());
				//Try pick terrain vertex
				if (_pickedEntity) {
					TerrainComponent* terrain = _pickedEntity->GetComponent<TerrainComponent>();
					if (terrain) {
						Vec2i coord = terrain->GetRayIntersectionTraingle(ray);
						if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_HEIGHT && coord.x >= 0) {
							terrain->ModifyHeight(coord, GetTerrainEditorOpacity(), GetTerrainEditorBrushSize());
							terrain->UpdateMesh();
							terrain->UpdateVegetables();
						}
						if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_TEXTURES && coord.x >= 0) {
							terrain->ModifyTexture(Vec2i(coord.y, coord.x),
								GetTerrainEditorOpacity(),
								GetTerrainEditorBrushSize(),
								GetTerrainEditorTextureIndex());
							terrain->UpdateTextureMasks();
						}
						if (GetTerrainEditorMode() == TERRAIN_EDITOR_EDIT_MODE_GRASS && coord.x >= 0) {
							terrain->ModifyGrass(Vec2i(coord.x, coord.y),
								GetTerrainEditorOpacity(),
								GetTerrainEditorVegetableIndex());
							terrain->UpdateVegetables();
						}
					}
				}

				std::vector<RayHit> hits;

				Entity* picked = nullptr;
				for (auto entity : renderer->GetEntitiesToDraw()) {
					AABB bb = entity->GetAABB(false);

					float len = ray.GetHitdistance(bb);
					if (len < 10000.f) {
						RayHit newhit(ray.GetHitdistance(bb), Vec3(0, 0, 0), entity);
						hits.push_back(newhit);
					}
				}

				std::sort(hits.begin(), hits.end(), [](const RayHit& a, const RayHit& b) { return a.GetDistance() < b.GetDistance(); });

			
				if (hits.size() > 0) {
					SetPickedEntity((Entity*)hits[0].GetHitObject());
					insp->SetShowingEntity((Entity*)hits[0].GetHitObject());
				}
			}
		}
	}
}

void EditorLayer::OnMouseButtonUp(const VSGE::EventMouseButtonUp& mbu) {
	if (mbu.GetMouseButton() == MOUSE_BUTTON_RIGHT) {
		InputState.right_btn_hold = false;
	}
	if (mbu.GetMouseButton() == MOUSE_BUTTON_LEFT) {
		InputState.left_btn_hold = false;
	}
}

void EditorLayer::OnKeyUp(const VSGE::EventKeyButtonUp& kbd) {
	switch (kbd.GetKeyCode()) {
	case KEY_CODE_LCTRL:
		InputState.isLCrtlHold = false;
		break;
	}
}

void EditorLayer::OnKeyDown(const VSGE::EventKeyButtonDown& kbd) {
	ImGuiLayer* imgui = ImGuiLayer::Get();
	TimePerf* time = TimePerf::Get();
	Vec3 cam_pos_offset(0.f);
	switch (kbd.GetKeyCode()) {
	case KEY_CODE_BACKSPACE:
		if(imgui->GetWindow<FileBrowserWindow>()->IsInFocus())
			imgui->GetWindow<FileBrowserWindow>()->cd_up();
		break;
	case KEY_CODE_F5:
		imgui->GetWindow<ToolbarWindow>()->PlayScene();
		break;
	case KEY_CODE_F6:
		imgui->GetWindow<ToolbarWindow>()->PauseScene();
		break;
	case KEY_CODE_DELETE:
		if (_pickedEntity != nullptr) {
			_pickedEntity->Destroy();
			imgui->GetWindow<InspectorWindow>()->SetShowingEntity(nullptr);
			_pickedEntity = nullptr;
		}
		break;
	case KEY_CODE_LCTRL:
		InputState.isLCrtlHold = true;
		break;
	case KEY_CODE_W:
		cam_pos_offset = mEditorCamera->GetFront() * MONEMENT_COEFF* time->GetDeltaTime();
		break;
	
	case KEY_CODE_S:
		if (InputState.isLCrtlHold) {
			ImGuiLayer::Get()->GetMenu<File_Menu>()->OnSave();
		}else
			cam_pos_offset = mEditorCamera->GetFront() * -MONEMENT_COEFF * time->GetDeltaTime();
		break;

	case KEY_CODE_A:
		cam_pos_offset = mEditorCamera->GetRight() * -MONEMENT_COEFF * time->GetDeltaTime();
		break;

	case KEY_CODE_D:
		cam_pos_offset = mEditorCamera->GetRight() * MONEMENT_COEFF * time->GetDeltaTime();
		break;
	}
	Vec3 right = mEditorCamera->GetRight();
	mEditorCamera->SetPosition(mEditorCamera->GetPosition() + cam_pos_offset);
}

void EditorLayer::OnFileEvent(const VSGE::FileChageEvent& fce) {
	ImGuiLayer::Get()->GetWindow<FileBrowserWindow>()->UpdateDirectoryContent();

	if (fce.GetActionType() == FCAT_ADDED) {
		//Try to add this file as resource
		ResourceCache::Get()->AddResourceFile(fce.GetAbsFilePath());
	}
	if (fce.GetActionType() == FCAT_DELETED) {
		Resource* res = ResourceCache::Get()->GetResourceWithFilePath(fce.GetAbsFilePath());
		if (res) {
			ResourceCache::Get()->RemoveResource(res);
			delete res;
		}
	}
	if (fce.GetActionType() == FCAT_RENAMED) {
		//Try to find this file as resource
		//To change resource name
		Resource* res = ResourceCache::Get()->GetResourceWithFilePath(fce.GetAbsFilePath());
		if (res) {
			DataDescription res_desc = res->GetDataDescription();
			res_desc.file_path = fce.GetNewAbsFilePath();
			uint32 filename_pos = 0;
			uint32 ext_pos = 0;

			for (uint32 i = 0; i < res_desc.file_path.size(); i++) {
				char cur_char = res_desc.file_path[i];
				if (cur_char == '\\' || cur_char == '/')
					filename_pos = i;
				if (cur_char == '.')
					ext_pos = i;
			}

			std::string res_name = "";
			for (uint32 i = filename_pos + 1; i < ext_pos; i++) {
				char cur_char = res_desc.file_path[i];
				res_name.push_back(cur_char);
			}

			res->SetName(res_name);
			res->SetDataDescription(res_desc);
		}
		else {
			//Try to add this file as resource
			ResourceCache::Get()->AddResourceFile(fce.GetNewAbsFilePath());
		}
	}
}

void EditorLayer::OnWindowClose(const VSGE::EventWindowClose& close) {
	Application::Get()->Stop();
}

void EditorLayer::OnMessageEvent(const VSGE::MessageEvent& me) {
	ConsoleWindow* cw = ImGuiLayer::Get()->GetWindow<ConsoleWindow>();

	if(cw)
		cw->addMsg((VSGE::MessageEvent*)&me);
}