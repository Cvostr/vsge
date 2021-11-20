#pragma once

#include <Engine/ApplicationLayer.hpp>
#include "Scene.hpp"

namespace VSGE {

	class SceneLoadedEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventSceneLoaded)
		SceneLoadedEvent(){}
	};

	class SceneLoadBeginEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventSceneLoadBegin)
		SceneLoadBeginEvent() {}
	};

	class OnSceneModifiedEvent : public IEvent {
	public:
		EVENT_CLASS_TYPE(EventType::EventSceneModified)
		OnSceneModifiedEvent() {}
	};

	class SceneLayer : public VSGE::IApplicationLayer {
	private:
		std::vector<Scene*> _scenes;

		byte* _scene_backup_data;
		uint32 _scene_backup_size;

		static SceneLayer* _this;
	public:
		SceneLayer();

		static SceneLayer* Get() {
			return _this;
		}

		void AddScene();
		/// <summary>
		/// Get vector of all created scenes
		/// </summary>
		/// <returns></returns>
		std::vector<Scene*>& GetScenes();
		/// <summary>
		/// Get first rendering scene
		/// </summary>
		/// <returns></returns>
		Scene* GetMainScene();
		/// <summary>
		/// Start scene execution, call OnStart() on all objects
		/// </summary>
		void StartScene(uint32 index = 0);
		/// <summary>
		/// Stop scene execution and call OnStop() on all objects
		/// </summary>
		void StopScene(uint32 index = 0);
		/// <summary>
		/// Pause scene execution
		/// </summary>
		void PauseScene(uint32 index = 0);

		bool IsSceneRunning(uint32 index = 0);
		bool IsScenePaused(uint32 index = 0);

		void OnAttach();
		void OnDetach();
		void OnUpdate();

		void BackupScene();
		void BackupScene(byte** data, uint32& size);
		void RestoreScene();
		void RestoreScene(byte* data, uint32 size);

		void LoadSceneYAML(byte* data, uint32 index = 0);
		void LoadSceneBinary(byte* data, uint32 size, uint32 index = 0);
	};
}