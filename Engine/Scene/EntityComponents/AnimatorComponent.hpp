#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/AnimationResource.hpp>
#include <vector>
#include <Math/Mat4.hpp>
#include <Scene/Entity.hpp>

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
		uint32 descriptor_id;
		
		AnimatorComponent();

		std::vector<AnimationCoeff>& GetAnimations() {
			return _animations;
		}

		void NewAnimation();

		bool IsPlaying() {
			return _playing;
		}

		void Play();

		void Pause();

		void Stop();

		void updateNodeTransform(Entity* child, const Mat4& parent);

		void OnPreRender();
		
		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_ANIMATOR, "Animator")
	};
}