#pragma once

#include <Engine/ApplicationLayer.hpp>
#include "Scene.hpp"

namespace VSGE {
	class SceneLayer : public VSGE::IApplicationLayer {
	private:
		Scene* _scene;

		bool _scene_running;
		bool _scene_paused;

		void CallOnStart(Entity* entity);
		void CallOnUpdate(Entity* entity);
		void CallOnStop(Entity* entity);

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
	};
}