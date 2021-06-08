#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Color.hpp>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	enum LightType {
		LIGHT_TYPE_NONE,
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
	public:

		LightsourceComponent() : 
			_intensity(1.f),
			_range(5.5f),
			_spot_angle(12.5f),
			_lightType(LIGHT_TYPE_DIRECTIONAL)
		{}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_LIGHTSOURCE, "Light Source")

		LightType& GetLightType() { return _lightType; }
		Color& GetColor() { return _lightColor; }

		float& GetIntensity() { return _intensity; }
		float& GetRange() { return _range; }
		float& GetSpotAngle() { return _spot_angle; }

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);
	};
}