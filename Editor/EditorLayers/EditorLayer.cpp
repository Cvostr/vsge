#include "EditorLayer.hpp" 
#include <Core/FileLoader.hpp>
#include <Engine/Application.hpp>
#include <Core/Time.hpp>
#include <filesystem>
#include "ImGuiLayer.hpp"
#include <Core/VarTypes/String.hpp>

#include "../Windows/SceneWindow.hpp"
#include "../Windows/InspectorWindow.hpp"
#include "../Windows/BrowserWindow.hpp"
#include "../Windows/SceneViewWindow.hpp"
#include "../Windows/ToolbarWindow.hpp"
#include "../Windows/ConsoleWindow.hpp"
#include "../Windows/EditorSettingsWindow.hpp"
#include "../Windows/EnvironmentSettingsWindow.hpp"
#include "../Windows/ProjectSettingsWindow.hpp"
#include "../Windows/ProjectBuildingWindow.hpp"
#include <Misc/DialogWindows.hpp>

#include <InspectorInterfaces/EntityComponents/EntityComponents.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/SceneLayer.hpp>

#include "../Menus/File_Menu.hpp"
#include "../Menus/Edit_Menu.hpp"
#include "../Menus/Windows_Menu.hpp"

#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <MonoScripting/MonoScriptStorage.hpp>
#include <MonoScripting/MonoScriptCompile.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Input/Input.hpp>
#include <ImGuizmo.h>

using namespace Mpi;

#define MONEMENT_COEFF 40.3f

using namespace VSGEditor;
using namespace VSGE;
namespace fs = std::filesystem;

EditorLayer* EditorLayer::_this = nullptr;

EditorLayer::EditorLayer() {
	mEditorCamera = new VSGE::Camera;
	mEditorCamera->SetFarPlane(5000);
	_this = this;

	CameraState.cam_yaw = 0;
	CameraState.cam_pitch = 0;

	_pickedEntity = nullptr;
	_transformMode = 7;
	_camera_mode = EDITOR_CAMERA_MODE_EDIT_CAMERA;
	_terrain_editor = new TerrainThreadedEditor;
	_terrain_editor->Run();
}

EditorLayer::~EditorLayer() {
	delete mEditorCamera;
	delete mResourcesWatcher;
	delete _terrain_editor;
}

void EditorLayer::OnAttach() {

}
void EditorLayer::OnUpdate() {
	ImGuiLayer* imgui = ImGuiLayer::Get();
	if (imgui->GetWindow<SceneViewWindow>())
		if (imgui->GetWindow<SceneViewWindow>()->IsInFocus()) {

			TimePerf* time = TimePerf::Get();
			Vec3 cam_pos_offset(0.f);
			if (Input::Get()->IsKeyHold(KEY_CODE_W))
				cam_pos_offset = mEditorCamera->GetFront() * MONEMENT_COEFF * time->GetDeltaTime();

			if (Input::Get()->IsKeyHold(KEY_CODE_A))
				cam_pos_offset = mEditorCamera->GetRight() * -MONEMENT_COEFF * time->GetDeltaTime();

			if (Input::Get()->IsKeyHold(KEY_CODE_D))
				cam_pos_offset = mEditorCamera->GetRight() * MONEMENT_COEFF * time->GetDeltaTime();

			if (Input::Get()->IsKeyHold(KEY_CODE_S))
				cam_pos_offset = mEditorCamera->GetFront() * -MONEMENT_COEFF * time->GetDeltaTime();

			mEditorCamera->SetPosition(mEditorCamera->GetPosition() + cam_pos_offset);
		}
}
void EditorLayer::OnDetach() {

}

bool EditorLayer::OpenProject(const Project& project) {
	_project = project;

	mResourcesWatcher = new FilesystemWatcher(File(project.GetAssetsDirectory()), true);
    mResourcesWatcher->setEventHandler([] (const FilesystemWatcherEvent& event) {
		FilesystemEvent* fc = new FilesystemEvent (event);
		Application::Get()->QueueEvent(fc);
    });

	ResourceCache::Get()->AddResourceDir(project.GetAssetsDirectory());

	ImGuiLayer::Get()->AddWindow(new SceneWindow);
	ImGuiLayer::Get()->AddWindow(new InspectorWindow);
	ImGuiLayer::Get()->AddWindow(new ConsoleWindow);
	ImGuiLayer::Get()->AddWindow(new FileBrowserWindow(project.GetAssetsDirectory()));
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

	ProjectBuildingWindow* pbw = new ProjectBuildingWindow;
	pbw->Hide();
	ImGuiLayer::Get()->AddWindow(pbw);

	ImGuiLayer::Get()->AddMenu(new File_Menu);
	ImGuiLayer::Get()->AddMenu(new Edit_Menu);
	ImGuiLayer::Get()->AddMenu(new Windows_Menu);

	VulkanRenderer::Get()->SetScene(SceneLayer::Get()->GetScenes()[0]);

	ImGuiLayer::Get()->SetDrawWindows(false);

	MonoScriptStorage::Get()->ClearScripts();
	MonoScriptStorage::Get()->BuildScriptList(project.GetAssetsDirectory());
	MonoScriptStorage::Get()->Compile();

	return false;
}

Project& EditorLayer::GetProject() {
	return _project;
}

void EditorLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::EventMouseMotion>(event, EVENT_FUNC(EditorLayer::OnMouseMotion));
	DispatchEvent<VSGE::EventWindowClose>(event, EVENT_FUNC(EditorLayer::OnWindowClose));
	DispatchEvent<VSGE::EventMouseScrolled>(event, EVENT_FUNC(EditorLayer::OnMouseScroll));
	DispatchEvent<VSGE::EventMouseButtonDown>(event, EVENT_FUNC(EditorLayer::OnMouseButtonDown));
	DispatchEvent<VSGE::EventMouseButtonUp>(event, EVENT_FUNC(EditorLayer::OnMouseButtonUp));
	DispatchEvent<VSGE::EventKeyButtonUp>(event, EVENT_FUNC(EditorLayer::OnKeyUp));
	DispatchEvent<VSGE::EventKeyButtonDown>(event, EVENT_FUNC(EditorLayer::OnKeyDown));
	DispatchEvent<VSGE::FilesystemEvent>(event, EVENT_FUNC(EditorLayer::fileEvent));
	DispatchEvent<VSGE::MessageEvent>(event, EVENT_FUNC(EditorLayer::OnMessageEvent));
	DispatchEvent<VSGE::ScriptCompilationDoneEvent>(event, EVENT_FUNC(EditorLayer::OnScriptCompiledEvent));
	DispatchEvent<VSGE::ScriptCompilationBeginEvent>(event, EVENT_FUNC(EditorLayer::OnScriptBeginEvent));
	DispatchEvent<VSGE::SceneLoadBeginEvent>(event, EVENT_FUNC(EditorLayer::OnSceneLoadBeginEvent));
	DispatchEvent<VSGE::SceneLoadedEvent>(event, EVENT_FUNC(EditorLayer::OnSceneLoadedEvent));
	DispatchEvent<VSGE::EventSceneViewResized>(event, EVENT_FUNC(EditorLayer::OnSceneViewResizedEvent));
}

void EditorLayer::OnMouseMotion(const VSGE::EventMouseMotion& motion) {
	ImGuiLayer* imgui = ImGuiLayer::Get();
	SceneViewWindow* win = imgui->GetWindow<SceneViewWindow>();
	if (win) {
		if (win->IsInFocus() && win->isInsideWindow(motion.GetMouseX(), motion.GetMouseY())
			&& Input::Get()->IsMouseButtonHold(MouseButton::MOUSE_BUTTON_RIGHT)) {

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
		if (win->IsInFocus() && win->isInsideWindow(motion.GetMouseX(), motion.GetMouseY())
			&& Input::Get()->IsMouseButtonHold(MouseButton::MOUSE_BUTTON_LEFT)) {
			Ray ray;
			if (GetPickingRay(ray)) {

				if (_pickedEntity) {
					TerrainComponent* terrain = _pickedEntity->GetComponent<TerrainComponent>();
					if (terrain) {
						_terrain_editor->QueueRay(ray);
					}
				}
			}
		}
	}
}

void EditorLayer::OnMouseScroll(const VSGE::EventMouseScrolled& scroll) {
	ImGuiLayer* imgui = ImGuiLayer::Get();
	SceneViewWindow* win = imgui->GetWindow<SceneViewWindow>();
	if (win) {
		Vec2i cursor_pos = Input::Get()->GetMouseCursorPos();
		uint32 cursorx = cursor_pos.x;
		uint32 cursory = cursor_pos.y;
		if (win->IsInFocus() && win->isInsideWindow(cursorx, cursory)) {
			Vec3 cam_front = mEditorCamera->GetFront() * (float)scroll.GetOffsetY();
			Vec3 new_pos = mEditorCamera->GetPosition() + cam_front;
			mEditorCamera->SetPosition(new_pos);
		}
	}
}

bool EditorLayer::GetPickingRay(VSGE::Ray& result) {
	Vec2i cursor_pos = Input::Get()->GetMouseCursorPos();
	int32 cursorx = cursor_pos.x;
	int32 cursory = cursor_pos.y;

	ImGuiLayer* imgui = ImGuiLayer::Get();
	SceneViewWindow* win = imgui->GetWindow<SceneViewWindow>();

	if (win && _camera_mode == EDITOR_CAMERA_MODE_EDIT_CAMERA) {
		if (!win->IsTransformGizmoUsed() &&
			win->IsInFocus() &&
			win->isInsideWindow(cursorx, cursory)) {
			int relx = cursorx - (int)win->GetPos().x;
			int rely = -cursory + (int)win->GetPos().y + (int)win->GetSize().y;

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
					return false;
			}

			Vec2 crpos = Vec2((float)relx / win->GetSize().x,
				(float)rely / win->GetSize().y
			);

			Vec3 world_pos = mEditorCamera->ScreenPointToWorldPoint(crpos);
			Vec3 dir = world_pos - mEditorCamera->GetPosition();

			result = Ray(mEditorCamera->GetPosition(), dir.GetNormalized());
			return true;
		}
	}
	return false;
}

void EditorLayer::OnMouseButtonDown(const VSGE::EventMouseButtonDown& mbd) {
	if (mbd.GetMouseButton() == MOUSE_BUTTON_LEFT) {
		ImGuiLayer* imgui = ImGuiLayer::Get();
		InspectorWindow* insp = imgui->GetWindow<InspectorWindow>();
		VulkanRenderer* renderer = VulkanRenderer::Get();

		//Try to pick object
		Ray ray;
		if (GetPickingRay(ray)) {

			if (_pickedEntity) {
				TerrainComponent* terrain = _pickedEntity->GetComponent<TerrainComponent>();
				if (terrain) {
					_terrain_editor->SetTerrain(terrain);
					_terrain_editor->QueueRay(ray);
				}
				else
					_terrain_editor->SetTerrain(nullptr);
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
			//triangle intersection
			float min_distance = MAX_FLOAT;
			if (hits.size() == 1) {
				SetPickedEntity((Entity*)hits[0].GetHitObject());
				insp->SetShowingEntity((Entity*)hits[0].GetHitObject());
			}
			else
			{
				for (auto& hit : hits) {
					Entity* ent = (Entity*)hit.GetHitObject();
					Mat4 transform = ent->GetWorldTransform();
					MeshComponent* mesh_comp = ent->GetComponent<MeshComponent>();
					if (!mesh_comp)
						continue;

					Mesh* entity_mesh = mesh_comp->GetMesh();

					if (!entity_mesh)
						continue;

					for (uint32 v_i = 0; v_i < entity_mesh->GetTrianglesCount(); v_i++) {
						Vec3 v0, v1, v2;
						entity_mesh->GetTriangle(v_i, v0, v1, v2);

						Vec4 v01 = transform * Vec4(v0, 1);
						Vec4 v11 = transform * Vec4(v1, 1);
						Vec4 v21 = transform * Vec4(v2, 1);

						float current_distance;
						Vec2 current_pos;
						bool intersects = ray.IntersectTriangle(v01.Vec3(), v11.Vec3(), v21.Vec3(), current_distance, current_pos);
						if (intersects && current_distance < min_distance) {
							min_distance = current_distance;
							SetPickedEntity(ent);
							insp->SetShowingEntity(ent);
						}
					}
				}
			}
		}
	}
}

void EditorLayer::OnMouseButtonUp(const VSGE::EventMouseButtonUp& mbu) {

}

void EditorLayer::OnKeyUp(const VSGE::EventKeyButtonUp& kbd) {

}

void EditorLayer::OnKeyDown(const VSGE::EventKeyButtonDown& kbd) {
	ImGuiLayer* imgui = ImGuiLayer::Get();
	TimePerf* time = TimePerf::Get();

	switch (kbd.GetKeyCode()) {
	case KEY_CODE_BACKSPACE:
		if(imgui->GetWindow<FileBrowserWindow>())
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
	
	case KEY_CODE_S:
		if (Input::Get()->IsKeyHold(KEY_CODE_LCTRL)) {
			ImGuiLayer::Get()->GetMenu<File_Menu>()->OnSave();
		}
		break;

	case KEY_CODE_Q:
		if (Input::Get()->IsKeyHold(KEY_CODE_LCTRL)) {
			if (_camera_mode == EDITOR_CAMERA_MODE_EDIT_CAMERA)
				_camera_mode = EDITOR_CAMERA_MODE_GAME_CAMERA;
			else if (_camera_mode == EDITOR_CAMERA_MODE_GAME_CAMERA)
				_camera_mode = EDITOR_CAMERA_MODE_EDIT_CAMERA;
		}
		break;
	}
}

void EditorLayer::OnSceneViewResizedEvent(const VSGE::EventSceneViewResized& svr) {
	VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
    renderer->ResizeOutput((uint32)svr.GetSizeX(), (uint32)svr.GetSizeY());
}

void EditorLayer::fileEvent(const FilesystemEvent& event) {
	ImGuiLayer::Get()->GetWindow<FileBrowserWindow>()->UpdateDirectoryContent();
	Mpi::FilesystemWatcherEvent fwe = event.getFilesystemWatcherEvent();
	std::string fileAbsPath = fwe.getFile().getAbsolutePath();

	if (fwe.getAction() == FILESYSTEM_ACTION_MODIFY) {
		if (MonoScriptStorage::Get()->GetScriptWithFilePath(fileAbsPath))
			MonoScriptStorage::Get()->Compile();
	}
	if (fwe.getAction() == FILESYSTEM_ACTION_CREATE) {
		//Try to add this file as resource
		ResourceCache::Get()->AddResourceFile(fileAbsPath);
		std::string fileExt = fwe.getFile().getExtension();
		if (fileExt == "cs") {
			MonoScriptStorage::Get()->AddScript(fileAbsPath);
			MonoScriptStorage::Get()->Compile();
		}
	}
	if (fwe.getAction() == FILESYSTEM_ACTION_DELETE) {
		Resource* res = ResourceCache::Get()->GetResourceWithFilePath(fileAbsPath);
		if (res) {
			res->Release();
			for (uint32 subresource_i = 0; subresource_i < res->GetSubresources().size(); subresource_i++) {
				Resource* subresource = res->GetSubresources()[subresource_i];
				subresource->Release();
				ResourceCache::Get()->RemoveResource(subresource);
				delete subresource;
			}
			ResourceCache::Get()->RemoveResource(res);
			delete res;
		}
		else {
			if (fwe.getFile().getExtension() == "cs") {
				MonoScriptStorage::Get()->RemoveScript(fileAbsPath);
				MonoScriptStorage::Get()->Compile();
			}
		}
	}
	if (fwe.getAction() == FILESYSTEM_ACTION_RENAMED) {
		std::string oldFileAbsPath = fwe.getOldFile().getAbsolutePath();
		//Try to find this file as resource
		//To change resource name
		Resource* res = ResourceCache::Get()->GetResourceWithFilePath(oldFileAbsPath);
		if (res) {
			DataDescription res_desc = res->GetDataDescription();
			res_desc.file_path = fileAbsPath;
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
			ResourceCache::Get()->AddResourceFile(fileAbsPath);
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

void EditorLayer::OnScriptBeginEvent(const VSGE::ScriptCompilationBeginEvent& scbe) {
	ImGuiLayer::Get()->GetHoldOnWindow()->Show();
	ImGuiLayer::Get()->GetHoldOnWindow()->SetTaskDescription("Compiling scripts...");
}

void EditorLayer::OnScriptCompiledEvent(const VSGE::ScriptCompilationDoneEvent& scde) {
	MonoScriptStorage::Get()->SetScriptingReady();
	SceneLayer::Get()->GetMainScene()->GetRootEntity()->CallOnScriptChanged(0);
	//Reload scripts dll
	MonoScriptingLayer::Get()->GetScriptsBlob()->LoadFromFile("mono_temp.dll");
	//Hide HoldOn window
	ImGuiLayer::Get()->SetDrawWindows(true);
	ImGuiLayer::Get()->GetHoldOnWindow()->Hide();
	//Update all script components on scene
	SceneLayer::Get()->GetMainScene()->GetRootEntity()->CallOnScriptChanged(1);
}

void EditorLayer::OnSceneLoadBeginEvent(const VSGE::SceneLoadBeginEvent& slbe) {
	ImGuiLayer::Get()->GetHoldOnWindow()->Show();
	ImGuiLayer::Get()->GetHoldOnWindow()->SetTaskDescription("Loading scene...");
}

void EditorLayer::OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& sle) {
	ImGuiLayer::Get()->GetHoldOnWindow()->Hide();
}