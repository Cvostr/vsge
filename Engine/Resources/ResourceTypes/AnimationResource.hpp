#pragma once

#include "../Resource.hpp"
#include <Graphics/Animation.hpp>

namespace VSGE {
	class AnimationResource : public Resource {
	private:
		Animation* _animation;
	public:

		AnimationResource() {
			_animation = new Animation;
		}

		Animation* GetAnimation() { return _animation; }

		void PostLoad() {
			//_texture->CreateFromBuffer(mLoadedData, description.size);
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_ANIMATION;
		}

	};
}