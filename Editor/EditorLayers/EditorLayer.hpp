#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <System/FileWatcher.hpp>
#include <Resources/ResourceCache.hpp>
#include <Engine/WindowEvent.hpp>
#include <Core/Logger.hpp>
#include <System/FileWatcher.hpp>
#include <MonoScripting/MonoScriptCompile.hpp>

namespace VSGEditor {

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

		struct inp_state {
			bool left_btn_hold;
			bool right_btn_hold;
			int cursorx;
			int cursory;

			bool isLCrtlHold;

			inp_state() {
				isLCrtlHold = false;
			}
		}InputState;

		std::string _openedSceneFile;
		VSGE::Entity* _pickedEntity;
		int _transformMode;

	public:

		EditorLayer() {
			mScene = new VSGE::Scene;
			mEditorCamera = new VSGE::Camera;
			mEditorCamera->SetFarPlane(5000);
			mScene->NewScene();
			_this = this;
			mResourcesWatcher = new VSGE::FileWatcher;

			CameraState.cam_yaw = 0;
			CameraState.cam_pitch = 0;
			InputState.right_btn_hold = false;
			InputState.left_btn_hold = false;

			_pickedEntity = nullptr;
			_transformMode = 7;
		}

		~EditorLayer() {
			delete mScene;
			delete mEditorCamera;
			delete mResourcesWatcher;
		}

		std::string& GetOpenedSceneFile() {
			return _openedSceneFile;
		}

		int GetTransformMode() {
			return _transformMode;
		}

		void SetTransformMode(int mode) {
			_transformMode = mode;
		}

		const inp_state& GetInputState() {
			return InputState;
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

		VSGE::Entity* GetPickedEntity() {
			return _pickedEntity;
		}

		void SetPickedEntity(VSGE::Entity* entity) {
			_pickedEntity = entity;
		}

		bool OpenProjectDirectory(const std::string& dir_path);

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		void OnEvent(const VSGE::IEvent& event);
		void OnMouseMotion(const VSGE::EventMouseMotion& motion);
		void OnMouseScroll(const VSGE::EventMouseScrolled& scroll);
		void OnMouseButtonDown(const VSGE::EventMouseButtonDown& mbd);
		void OnMouseButtonUp(const VSGE::EventMouseButtonUp& mbu);
		void OnWindowClose(const VSGE::EventWindowClose& close);
		void OnKeyUp(const VSGE::EventKeyButtonUp& kbd);
		void OnKeyDown(const VSGE::EventKeyButtonDown& kbd);
		void OnFileEvent(const VSGE::FileChageEvent& fce);
		void OnMessageEvent(const VSGE::MessageEvent& me);
		void OnScriptCompiledEvent(const VSGE::ScriptCompilationDoneEvent& scde);
	};
}