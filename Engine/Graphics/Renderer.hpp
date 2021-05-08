#pragma once

#include <Scene/Scene.hpp>
#include "Texture.hpp"

namespace VSGE {
	class IRenderer {
	protected:
		uint32 mOutputWidth;
		uint32 mOutputHeight;
		
		Scene* mScene;
		Texture* mOutput;

		tEntityList mEntitiesToRender;
	public:

		IRenderer() : 
			mScene(nullptr),
			mOutput(nullptr),
			mOutputWidth(1280),
			mOutputHeight(720)
		{}

		void SetScene(Scene* scene) { mScene = scene; }

		Texture* GetOutputTexture() {
			return mOutput;
		}

		virtual void DrawScene() = 0;

		void CreateRenderList();
		void ProcessEntity(Entity* entity);
	};
}