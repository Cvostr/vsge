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
#include <Base/Project.hpp>

namespace VSGEditor {

	enum EditorCameraMode {
		EDITOR_CAMERA_MODE_EDIT_CAMERA,
		EDITOR_CAMERA_MODE_GAME_CAMERA
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

		std::string _openedSceneFile;
		VSGE::Entity* _pickedEntity;
		int _transformMode;
		EditorCameraMode _camera_mode;
		Project _project;
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

			_pickedEntity = nullptr;
			_transformMode = 7;
			_camera_mode = EDITOR_CAMERA_MODE_EDIT_CAMERA;
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

		EditorCameraMode GetEditorCameraMode() {
			return _camera_mode;
		}

		void SetEditorCameraMode(EditorCameraMode mode) {
			_camera_mode = mode;
		}

		bool OpenProject(const Project& project);
		const Project& GetProject();

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
		void OnScriptBeginEvent(const VSGE::ScriptCompilationBeginEvent& scbe);
		void OnScriptCompiledEvent(const VSGE::ScriptCompilationDoneEvent& scde);
	};
}