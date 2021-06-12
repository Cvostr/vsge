#include "AnimatorComponent.hpp"
#include <Core/Time.hpp>
#include <Math/MatrixTransform.hpp>

using namespace YAML;
using namespace VSGE;

AnimatorComponent::AnimatorComponent() :
	_playing(false),
	_startTime(0),
    _current_time(0)
{}

void AnimatorComponent::AddNewAnimation() {
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
        if(anim._animResource.GetResource())
		    anim._animResource.GetResource()->Load();
	}
}

void AnimatorComponent::Pause() {
	_playing = false;
}

void AnimatorComponent::Stop() {
	_playing = false;
}

void AnimatorComponent::UpdateNodeTransform(Entity* child) {
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
        UpdateNodeTransform(child->GetChildren()[i]);
    }
}

void AnimatorComponent::OnPreRender() {
    if (_playing) {
        TimePerf* time = TimePerf::Get();
        _current_time = static_cast<double>(time->GetCurrentTime()) / 1000 - _startTime;
        
        UpdateNodeTransform(GetEntity());
    }
}

void AnimatorComponent::Serialize(YAML::Emitter& e) {
    e << YAML::Key << "anims" << YAML::Value << YAML::BeginSeq;
    for (auto& anim : _animations) {
        e << BeginMap;
        e << Key << "resource" << Value << anim._animResource.GetResourceName();
        e << Key << "coeff" << Value << anim.coeff;
        e << YAML::EndMap; // Anim resource
    }
    e << YAML::EndSeq;
}
void AnimatorComponent::Deserialize(YAML::Node& entity) {
    YAML::Node anims = entity["anims"];
    for (auto& anim : anims) {
        std::string resource_name = anim["resource"].as<std::string>();
        float coeff = anim["coeff"].as<float>();

        AnimationCoeff anim_coeff;
        anim_coeff._animResource.SetResource(resource_name);
        anim_coeff.coeff = coeff;
        _animations.push_back(anim_coeff);
    }
}