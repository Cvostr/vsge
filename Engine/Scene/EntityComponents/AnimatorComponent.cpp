#include "AnimatorComponent.hpp"
#include <Core/Time.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

AnimatorComponent::AnimatorComponent() :
	_playing(false),
	_startTime(0),
    _current_time(0)
{}

void AnimatorComponent::AddNewAnimation() {
	AnimationCoeff coeff;
	coeff.coeff = 1;
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

bool AnimatorComponent::IsPlaying() {
    return _playing;
}

void AnimatorComponent::Pause() {
	_playing = false;
}

void AnimatorComponent::Stop() {
	_playing = false;
}

void AnimatorComponent::UpdateNodeTransform(Entity* child) {
    if (!child && !_playing) return;
    //iterate over all loaded animations
    uint32 index = 0;
    for (auto animation : _anims_ready) {
        //Time in animation ticks
        double ticks = animation->GetTPS() * _current_time;
        //Calculate current animation time
        double animTime = fmod(ticks, animation->GetDuration());
        //Get channel with entity name
        AnimationChannel* cha = animation->GetChannelByEntityName(child->GetName());
        if (cha) {
            child->SetPosition(cha->getPositionInterpolated(animTime, animation->GetTPS()));
            child->SetScale(cha->getScaleInterpolated(animTime, animation->GetTPS()));
            child->SetRotation(cha->getRotationInterpolated(animTime, animation->GetTPS()).Conjugate());
        }
        index++;
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
        
        _anims_ready.clear();

        for (auto& anim : _animations) {
            ResourceReference* ref = &anim._animResource;
            AnimationResource* resource = ref->GetResource<AnimationResource>();
            if (!resource)
                continue;
            //if anim isn't ready, then queue it to load
            if (!resource->IsReady()) {
                resource->Load();
                continue;
            }
            //Mark animation used in this frame
            resource->Use();
            //Get animation class from resource
            _anims_ready.push_back(resource->GetAnimation());
        }
        //process root entity
        UpdateNodeTransform(GetEntity());
    }
}

void AnimatorComponent::Serialize(ByteSerialize& serializer) 
{
    //Записать количество анимационных групп
    uint32 anims_count = static_cast<uint32>(_animations.size());
    serializer.Serialize(anims_count);

    for (const auto& anim : _animations) 
    {
        serializer.Serialize(anim._animResource.GetId());
        serializer.Serialize(anim.coeff);
    }
}
void AnimatorComponent::Deserialize(ByteSolver& solver)
{
    //Считать количество анимационных групп
    uint32 count = solver.GetValue<uint32>();

    //Считать анимационные группы
    for (uint32 anim_i = 0; anim_i < count; anim_i++) 
    {
        Guid resourceId = solver.GetGuid();
        float coeff = solver.GetValue<float>();

        AnimationCoeff anim_coeff;
        anim_coeff._animResource.SetResource(resourceId);
        anim_coeff.coeff = coeff;
        _animations.push_back(anim_coeff);
    }
}
