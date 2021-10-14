#pragma once

#include <Scene/Scene.hpp>
#include <Math/Vec2.hpp>
#include "Texture.hpp"

namespace VSGE {
	class IRenderer {
	protected:
		uint32 mOutputWidth;
		uint32 mOutputHeight;
		
		Scene* mScene;
		Texture* mOutput;

		//Entities with mesh and material to render
		tEntityList _entitiesToRender;
		//Entities with lightsource
		tEntityList _lightsources;
		//Entities with shadowcasters
		tEntityList _shadowcasters;
		//Entities with particle emitter
		tEntityList _particleEmitters;
		//Entities with camera
		tEntityList _cameras;
		//Entities with terrain
		tEntityList _terrains;

	public:

		IRenderer() : 
			mScene(nullptr),
			mOutput(nullptr),
			mOutputWidth(1280),
			mOutputHeight(720)
		{}

		void SetScene(Scene* scene);
		Scene* GetScene();

		Texture* GetOutputTexture();
		Vec2i GetOutputSizes();

		virtual void ResizeOutput(uint32 width, uint32 height) = 0;

		const tEntityList& GetEntitiesToDraw() {
			return _entitiesToRender;
		}

		virtual void DrawScene(VSGE::Camera* cam) = 0;

		void CreateRenderList();
		void ProcessEntity(Entity* entity);
	};
}