#include "AnimationResource.hpp"
#include "../AnimationParser.hpp"

using namespace VSGE;

AnimationResource::AnimationResource() {
	_animation = new Animation;
}

AnimationResource::~AnimationResource() {
	delete _animation;
}

Animation* AnimationResource::GetAnimation() { return _animation; }

void AnimationResource::Release() {
	_animation->Release();
}

void AnimationResource::PostLoad() {
	ImportedAnimationFile iaf;
    iaf.loadFromBuffer(_loadedData, _description.size);
	iaf._Animation->CopyTo(_animation);
}