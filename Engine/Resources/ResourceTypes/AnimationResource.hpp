#pragma once

#include "../Resource.hpp"
#include <Graphics/Animation.hpp>

namespace VSGE {
	class AnimationResource : public Resource {
	private:
		Animation* _animation;
	public:

		AnimationResource();

		~AnimationResource();

		Animation* GetAnimation();

		void Release();

		void PostLoad();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_ANIMATION)

	};
}