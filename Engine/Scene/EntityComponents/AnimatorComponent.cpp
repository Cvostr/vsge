#include "AnimatorComponent.hpp"
#include <Core/Time.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

AnimatorComponent::AnimatorComponent() :
	_playing(false),
	_startTime(0),
    _current_time(0)
{}

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
	_startTime = static_cast<double>(time->GetCurrentTime()) / 1000;

	for (auto& anim : _animations) {
		//Queue resource loading
		anim._animResource.GetResource()->Load();
	}
}

void AnimatorComponent::Pause() {
	_playing = false;
}

void AnimatorComponent::Stop() {
	_playing = false;
}

void AnimatorComponent::updateNodeTransform(Entity* child) {
    if (!child && !_playing) return;

    //Assign base node transform
    Mat4 abs = child->GetLocalTransform();
    
    for (auto& anim : _animations) {
        ResourceReference* ref = &anim._animResource;
        if (!ref->GetResource())
            continue;

        AnimationResource* resource = ref->GetResource<AnimationResource>();

        if (!ref->GetResource()->IsReady()) {
            ref->GetResource()->Load();
            continue;
        }

        Animation* animation = resource->GetAnimation();
        //Time in animation ticks
        double ticks = animation->GetTPS() * _current_time;
        //Calculate current animation time
        double animTime = fmod(ticks, animation->GetDuration());

        AnimationChannel* cha = animation->GetChannelByEntityName(child->GetName());
        if (cha) {
            child->SetPosition(cha->getPositionInterpolated(animTime));
            child->SetScale(cha->getScaleInterpolated(animTime));
            child->SetRotation(cha->getRotationInterpolated(animTime).Conjugate());
        }
    }
    
    //Go deeper in tree
    for (uint32 i = 0; i < child->GetChildrenCount(); i++) {
        updateNodeTransform(child->GetChildren()[i]);
    }
}

void AnimatorComponent::OnPreRender() {
    if (_playing) {
        TimePerf* time = TimePerf::Get();
        _current_time = static_cast<double>(time->GetCurrentTime()) / 1000 - _startTime;
        
        updateNodeTransform(GetEntity());
    }
}