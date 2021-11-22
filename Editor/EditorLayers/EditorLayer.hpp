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
#include <Scene/SceneLayer.hpp>
#include <Base/Project.hpp>
#include <Misc/TerrainThreadedEditor.hpp>

namespace VSGEditor {

	enum EditorCameraMode {
		EDITOR_CAMERA_MODE_EDIT_CAMERA,
		EDITOR_CAMERA_MODE_GAME_CAMERA
	};

	class EditorLayer : public VSGE::IApplicationLayer {
	private:
		static EditorLayer* _this;
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
		TerrainThreadedEditor* _terrain_editor;

		bool GetPickingRay(VSGE::Ray& result);
	public:

		EditorLayer();

		~EditorLayer();

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
		void OnSceneLoadBeginEvent(const VSGE::SceneLoadBeginEvent& slbe);
		void OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& sle);
	};
}