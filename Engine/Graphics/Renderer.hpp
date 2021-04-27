#pragma once

#include <Scene/Scene.hpp>
#include "Texture.hpp"

namespace VSGE {
	class IRenderer {
	private:
		Scene* mScene;
		Texture* mOutput;
	public:

		IRenderer() : 
			mScene(nullptr),
			mOutput(nullptr)
		{}

		void SetScene(Scene* scene) { mScene = scene; }

		Texture* GetOutputTexture() {
			return mOutput;
		}

		virtual void DrawScene() = 0;
	};
}