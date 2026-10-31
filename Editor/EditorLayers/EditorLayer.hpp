#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Resources/ResourceCache.hpp>
#include <Engine/WindowEvent.hpp>
#include <Core/Logger.hpp>
#include <MonoScripting/MonoScriptCompile.hpp>
#include <Scene/SceneLayer.hpp>
#include <Base/Project.hpp>
#include <Math/Ray.hpp>
#include <Base/Events.hpp>
#include <mpi/Filesystem/FilesystemWatcher.hpp>
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
		Mpi::FilesystemWatcher* mResourcesWatcher;
		TerrainThreadedEditor* _terrain_editor;

		struct {
			float cam_yaw;
			float cam_pitch;
		}CameraState;

		std::string _openedSceneFile;
		VSGE::Entity* _pickedEntity;
		int _transformMode;
		EditorCameraMode _camera_mode;
		Project _project;

		bool GetPickingRay(VSGE::Ray& result);
	public:

		EditorLayer();
		~EditorLayer();

		static EditorLayer* Get() {
			return _this;
		}

		VSGE::Camera* GetCamera();

		const std::string& GetOpenedSceneFile();
		void SetOpenedSceneFile(const std::string& path);
		
		int GetTransformMode();
		void SetTransformMode(int mode);

		VSGE::Entity* GetPickedEntity();
		void SetPickedEntity(VSGE::Entity* entity);

		EditorCameraMode GetEditorCameraMode();
		void SetEditorCameraMode(EditorCameraMode mode);

		bool OpenProject(const Project& project);
		Project& GetProject();

		void fileEvent(const VSGE::FilesystemEvent& event);

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
		void OnMessageEvent(const VSGE::MessageEvent& me);
		void OnScriptBeginEvent(const VSGE::ScriptCompilationBeginEvent& scbe);
		void OnScriptCompiledEvent(const VSGE::ScriptCompilationDoneEvent& scde);
		void OnSceneLoadBeginEvent(const VSGE::SceneLoadBeginEvent& slbe);
		void OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& sle);
		void OnSceneViewResizedEvent(const VSGE::EventSceneViewResized& svr);
	};
}