#pragma once

#include <Engine/ApplicationLayer.hpp>
#include "Scene.hpp"

namespace VSGE {
	class SceneLayer : public VSGE::IApplicationLayer {
	private:
		Scene* _scene;

		bool _scene_running;

		void CallOnStart(Entity* entity);
		void CallOnUpdate(Entity* entity);
		void CallOnStop(Entity* entity);
	public:
		SceneLayer();

		void SetWorkingScene(Scene* scene);

		Scene* GetWorkingScene();

		void StartScene();

		void StopScene();

		bool IsSceneRunning();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
	};
}