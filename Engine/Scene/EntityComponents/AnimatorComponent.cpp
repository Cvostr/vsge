#include "AnimatorComponent.hpp"
#include <Core/Time.hpp>

using namespace VSGE;

void AnimatorComponent::NewAnimation() {
	AnimationCoeff coeff;
	coeff.coeff = 1;
	coeff._animResource.SetResourceType(RESOURCE_TYPE_ANIMATION);
	_animations.push_back(coeff);
}

void AnimatorComponent::Play() {
	if (_playing)
		return;

	TimePerf* time = TimePerf::Get();

	_playing = true;
	_startTime = (static_cast<double>(time->GetCurrentTime()) / 1000);

	for (auto& anim : _animations) {
		//Queue resource loading
		anim._animResource.GetResource()->Load();
	}
}

void AnimatorComponent::Pause() {

}

void AnimatorComponent::Stop() {
	_playing = false;
}