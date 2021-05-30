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
		bool _playing;
		double _startTime;
	public:
		AnimatorComponent(){}

		std::vector<AnimationCoeff>& GetAnimations() {
			return _animations;
		}

		void NewAnimation();

		void Play();

		void Pause();

		void Stop();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_ANIMATOR, "Animator")
	};
}