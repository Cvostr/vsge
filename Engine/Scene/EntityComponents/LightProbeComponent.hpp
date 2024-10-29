#pragma once

#include "../IEntityComponent.hpp"
#include <Graphics/LightProbeImage.hpp>

namespace VSGE {

	class LightProbeComponent : public IEntityComponent {
	private:
		LightProbeImage* image;
		bool baked;
		float radius;
		float nearPlane;
		float farPlane;
	public:
		LightProbeComponent();
		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_LIGHT_PROBE, "Light Probe")

		LightProbeImage* GetImage() const;
		void SetImage(LightProbeImage* image);
		bool IsBaked() const;

		float GetNearPlane() const;
		void SetNearPlane(float nearPlane);

		float GetFarPlane() const;
		void SetFarPlane(float farPlane);

		float GetRadius() const;
		void SetRadius(float radius);

		bool LoadImage();
		void Bake();

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);
	};
}