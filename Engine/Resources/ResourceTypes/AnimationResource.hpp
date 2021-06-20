#pragma once

#include "../Resource.hpp"
#include <Graphics/Animation.hpp>
#include "../AnimationParser.hpp"

namespace VSGE {
	class AnimationResource : public Resource {
	private:
		Animation* _animation;
	public:

		AnimationResource() {
			_animation = new Animation;
		}

		~AnimationResource() {
			delete _animation;
		}

		Animation* GetAnimation() { return _animation; }

		void Release() {
			_animation->Release();
		}

		void PostLoad() {
			ImportedAnimationFile iaf;
			iaf.loadFromBuffer(_loadedData, _description.size);
			iaf._Animation->CopyTo(_animation);
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_ANIMATION;
		}

	};
}