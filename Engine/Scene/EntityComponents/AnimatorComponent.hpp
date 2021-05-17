#pragma once

#include "../IEntityComponent.hpp"
#include <vector>

namespace VSGE {
	class AnimatorComponent : public IEntityComponent {
	private:
	public:
		AnimatorComponent(){}

		EntityComponentType GetType() {
			return ENTITY_COMPONENT_ANIMATOR;
		}
	};
}