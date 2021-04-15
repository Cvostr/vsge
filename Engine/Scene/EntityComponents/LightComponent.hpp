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

	class LightComponent : public IEntityComponent {
	private:
		LightType mLightType;
		Color mLightColor;
		float mIntensity;
		float mRange;
	public:

	};
}