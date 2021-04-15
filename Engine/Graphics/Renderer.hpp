#pragma once

#include <Scene/Scene.hpp>

namespace VSGE {
	class IRenderer {
	private:
		Scene* mScene;
	public:

		void SetScene(Scene* scene) { mScene = scene; }

		virtual void DrawScene() = 0;
	};
}