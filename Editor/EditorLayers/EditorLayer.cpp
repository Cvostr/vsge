#include "EditorLayer.hpp" 
#include <Core/FileLoader.hpp>
#include <Engine/Application.hpp>
#include <Core/Time.hpp>

#include "ImGuiLayer.hpp"

#include "../Windows/SceneWindow.hpp"
#include "../Windows/InspectorWindow.hpp"
#include "../Windows/BrowserWindow.hpp"
#include "../Windows/SceneViewWindow.hpp"
#include "../Windows/ToolbarWindow.hpp"
#include "../Windows/ConsoleWindow.hpp"
#include "../Windows/EditorSettingsWindow.hpp"

#include "../Menus/File_Menu.hpp"
#include "../Menus/Windows_Menu.hpp"

#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

#include <Math/Ray.hpp>

#define MONEMENT_COEFF 40.3f

using namespace VSGEditor;
using namespace VSGE;

EditorLayer* EditorLayer::_this = nullptr;

void EditorLayer::OnAttach() {

}
void EditorLayer::OnUpdate() {

}
void EditorLayer::OnDetach() {

}

void EditorLayer::OpenProjectDirectory(const std::string& dir_path) {
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

	ImGuiLayer::Get()->AddMenu(new File_Menu);
	ImGuiLayer::Get()->AddMenu(new Windows_Menu);

	VulkanRenderer::Get()->SetScene(this->mScene);
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
				int rely = -InputState.cursory + win->GetPos().y + win->GetSize().y + WINDOW_MENU_OFFSET;

				Vec2 crpos = Vec2((float)relx / win->GetSize().x,
					(float)rely / win->GetSize().y
				);

				Vec3 world_pos = mEditorCamera->ScreenPointToWorldPoint(crpos);
				Vec3 dir = world_pos - mEditorCamera->GetPosition();

				VulkanRenderer* renderer = VulkanRenderer::Get();

				Ray ray(mEditorCamera->GetPosition(), dir.GetNormalized());

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
				else {
				//	SetPickedEntity(nullptr);
				//	insp->SetShowingEntity(nullptr);
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
	TimePerf* time = TimePerf::Get();
	Vec3 cam_pos_offset(0.f);
	switch (kbd.GetKeyCode()) {
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
}

void EditorLayer::OnWindowClose(const VSGE::EventWindowClose& close) {
	Application::Get()->Stop();
}

void EditorLayer::OnMessageEvent(const VSGE::MessageEvent& me) {
	ConsoleWindow* cw = ImGuiLayer::Get()->GetWindow<ConsoleWindow>();

	if(cw)
		cw->addMsg((VSGE::MessageEvent*)&me);
}