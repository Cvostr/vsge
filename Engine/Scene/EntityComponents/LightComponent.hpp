#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Color.hpp>
#include <Math/Vec3.hpp>
#include <Math/Mat4.hpp>
#include <Core/VarTypes/Base.hpp>
#include "../Camera.hpp"

#define MAX_SHADOW_CASCADES 10

namespace VSGE {

	enum LightType {
		LIGHT_TYPE_DIRECTIONAL,
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT
	};

	class LightsourceComponent : public IEntityComponent {
	private:
		LightType _lightType;
		Color _lightColor;
		float _intensity;
		float _range;
		float _spot_angle;

		bool _castShadows;
		float _shadowStrength;
		float _shadowsBias;
		uint32 _shadowsPCF;
	public:

		LightsourceComponent();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_LIGHTSOURCE, "Light Source")

		LightType& GetLightType();
		Color& GetColor();

		float& GetIntensity();
		float& GetRange();
		float& GetSpotAngle();

		bool GetCastShadows();
		void SetCastShadows(bool castShadows);

		float GetShadowStrength();
		void SetShadowStrength(float strength);

		float GetShadowsBias();
		void SetShadowsBias(float bias);

		uint32 GetShadowPCF();
		void SetShadowPCF(uint32 pcf);

		Vec3 GetDirection();

		Mat4* GetShadowcastMatrices(Camera* cam);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);
	};
}