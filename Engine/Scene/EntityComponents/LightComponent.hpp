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
		LightType mLightType;
		Color mLightColor;
		float mIntensity;
		float mRange;
	public:

		LightsourceComponent() : 
			mIntensity(1.f),
			mRange(5.5f),
			mLightType(LIGHT_TYPE_DIRECTIONAL)
		{}

		EntityComponentType GetType() {
			return ENTITY_COMPONENT_LIGHTSOURCE;
		}

		LightType& GetLightType() { return mLightType; }
		Color& GetColor() { return mLightColor; }

		float& GetIntensity() { return mIntensity; }
		float& GetRange() { return mRange; }
	};
}