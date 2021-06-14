#pragma once

#include <Engine/ApplicationLayer.hpp>
#include "Scene.hpp"

namespace VSGE {
	class SceneLayer : public VSGE::IApplicationLayer {
	private:
		Scene* _scene;
	public:
		void SetWorkingScene(Scene* scene);

		Scene* GetWorkingScene();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate();
	};
}