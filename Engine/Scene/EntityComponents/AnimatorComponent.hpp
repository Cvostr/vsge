#pragma once

#include "../IEntityComponent.hpp"
#include <vector>

namespace VSGE {
	class AnimatorComponent : public IEntityComponent {
	private:
	public:
		AnimatorComponent(){}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_ANIMATOR, "Animator")
	};
}