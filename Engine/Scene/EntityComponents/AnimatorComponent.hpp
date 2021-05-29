#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/AnimationResource.hpp>
#include <vector>

namespace VSGE {

	class AnimationCoeff {
	public:
		ResourceReference _animResource;
		float coeff;
	};

	class AnimatorComponent : public IEntityComponent {
	private:
		std::vector<AnimationCoeff> _animations;
	public:
		AnimatorComponent(){}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_ANIMATOR, "Animator")
	};
}