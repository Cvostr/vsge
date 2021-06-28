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
		std::vector<Animation*> _anims_ready;
		bool _playing;
		double _startTime;
		double _current_time;
	public:
		
		AnimatorComponent();

		std::vector<AnimationCoeff>& GetAnimations() {
			return _animations;
		}

		void AddNewAnimation();

		bool IsPlaying() {
			return _playing;
		}

		void Play();

		void Pause();

		void Stop();

		void UpdateNodeTransform(Entity* child);

		void OnPreRender();

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);
		
		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_ANIMATOR, "Animator")
	};
}