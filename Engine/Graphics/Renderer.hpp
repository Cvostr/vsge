#pragma once

#include <Scene/Scene.hpp>
#include <Math/Vec2.hpp>
#include "Texture.hpp"
#include "LightProbeImage.hpp"

namespace VSGE {

	struct LightProbeRenderRequest {

		LightProbeImage* image;
		Vec3 pos;
		float nearPlane;
		float farPlane;
	};

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

		tEntityList _probes;

	public:

		IRenderer();

		void SetScene(Scene* scene);
		Scene* GetScene();

		Texture* GetOutputTexture();
		Vec2i GetOutputSizes();

		virtual void ResizeOutput(uint32 width, uint32 height) = 0;

		const tEntityList& GetEntitiesToDraw();

		virtual void DrawScene(VSGE::Camera* cam) = 0;

		void CreateRenderList();
		void ProcessEntity(Entity* entity);

		virtual void EnqueueLightProbe(const LightProbeRenderRequest& component) = 0;
	};
}