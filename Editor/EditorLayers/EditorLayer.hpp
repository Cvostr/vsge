#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <System/FileWatcher.hpp>
#include <Resources/ResourceCache.hpp>
#include <Engine/WindowEvent.hpp>

namespace VSGEditor {

	class EditorSettings {

	};

	class EditorLayer : public VSGE::IApplicationLayer {
	private:
		static EditorLayer* _this;
		VSGE::Scene* mScene;
		VSGE::Camera* mEditorCamera;
		VSGE::FileWatcher* mResourcesWatcher;

		struct {
			float cam_yaw;
			float cam_pitch;
		}CameraState;

		struct {
			bool right_btn_hold;
			int cursorx;
			int cursory;
		}InputState;

	public:

		EditorLayer() {
			mScene = new VSGE::Scene;
			mEditorCamera = new VSGE::Camera;
			mScene->NewScene();
			_this = this;
			mResourcesWatcher = new VSGE::FileWatcher;

			CameraState.cam_yaw = 0;
			CameraState.cam_pitch = 0;
			InputState.right_btn_hold = false;
		}

		~EditorLayer() {
			delete mScene;
			delete mEditorCamera;
			delete mResourcesWatcher;
		}

		static EditorLayer* Get() {
			return _this;
		}

		VSGE::Scene* GetScene() {
			return mScene;
		}

		VSGE::Camera* GetCamera() {
			return mEditorCamera;
		}

		void OpenProjectDirectory(const std::string& dir_path);

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		void OnEvent(const VSGE::IEvent& event);
		void OnMouseMotion(const VSGE::EventMouseMotion& motion);
		void OnMouseScroll(const VSGE::EventMouseScrolled& scroll);
		void OnMouseButtonDown(const VSGE::EventMouseButtonDown& mbd);
		void OnMouseButtonUp(const VSGE::EventMouseButtonUp& mbu);
		void OnWindowClose(const VSGE::EventWindowClose& close);

	};
}