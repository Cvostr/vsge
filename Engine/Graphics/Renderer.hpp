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

		//Entities with mesh and material to render
		tEntityList mEntitiesToRender;
		//Entities with lightsource
		tEntityList mLightsources;
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

		virtual void DrawScene(VSGE::Camera* cam) = 0;

		void CreateRenderList();
		void ProcessEntity(Entity* entity);
	};
}