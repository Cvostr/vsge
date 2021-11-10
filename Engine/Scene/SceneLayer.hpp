#pragma once

#include <Engine/ApplicationLayer.hpp>
#include "Scene.hpp"

namespace VSGE {

	class OnSceneLoadedEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventSceneLoaded)
		OnSceneLoadedEvent(){}
	};

	class OnSceneModifiedEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventSceneModified)
		OnSceneModifiedEvent() {}
	};

	class SceneLayer : public VSGE::IApplicationLayer {
	private:
		Scene* _scene;

		bool _scene_running;
		bool _scene_paused;

		void CallOnStart(Entity* entity);
		void CallOnUpdate(Entity* entity);
		void CallOnStop(Entity* entity);

		byte* _scene_backup_data;
		uint32 _scene_backup_size;

		static SceneLayer* _this;
	public:
		SceneLayer();

		static SceneLayer* Get() {
			return _this;
		}

		void SetWorkingScene(Scene* scene);

		Scene* GetWorkingScene();
		/// <summary>
		/// Start scene execution, call OnStart() on all objects
		/// </summary>
		void StartScene();
		/// <summary>
		/// Stop scene execution and call OnStop() on all objects
		/// </summary>
		void StopScene();
		/// <summary>
		/// Pause scene execution
		/// </summary>
		void PauseScene();

		bool IsSceneRunning();
		bool IsScenePaused();

		void OnAttach();
		void OnDetach();
		void OnUpdate();

		void BackupScene();
		void BackupScene(byte** data, uint32& size);
		void RestoreScene();
		void RestoreScene(byte* data, uint32 size);
	};
}