#include "AnimatorComponent.hpp"
#include <Core/Time.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

AnimatorComponent::AnimatorComponent() :
	_playing(false),
	_startTime(0)
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
	_startTime = (static_cast<double>(time->GetCurrentTime()) / 1000);

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

void AnimatorComponent::updateNodeTransform(Entity* child, const Mat4& parent) {
    if (!child) return;

    //Assign base node transform
    Mat4 abs = child->GetLocalTransform();
    //Are we need to play animation
    Vec3 position = child->GetPosition();
    Vec3 scale = child->GetScale();
    Quat rotation = child->GetRotation();
    abs = GetTransform(child->GetPosition(), child->GetScale(), child->GetRotation()).transpose();
    //abs = GetTranslationMatrix(position).transpose() * GetRotationMatrix(rotation).transpose() * GetScaleMatrix(scale).transpose();
    //abs = GetTranslationMatrix(position) * GetRotationMatrix(rotation) * GetScaleMatrix(scale);
    /*AnimationResource* resource = _animations[0]._animResource.GetResource<AnimationResource>();

    if (resource != nullptr && _playing && resource->GetResourceType() == RESOURCE_STATE_LOADED) {
        //Try to find anim channel by label of node
        
       // AnimationChannel* cha = resource->GetAnimation()->getChannelByNodeName(prop->node_label);
        //if channel is found
        if (cha) {
            Mat4 transl = transpose(getTranslationMat(prop->translation));
            Mat4 _sca = transpose(getScaleMat(prop->scale));
            Mat4 rot = (getRotationMat(prop->rotation));
            //Multiply all matrices
            prop->abs = transl * rot * _sca;
        }
    }*/

    

    //Apply parent transform
    abs = parent * abs;

    child->SetWorldTransform(abs);
    

    //Go deeper in tree
    for (size_t i = 0; i < child->GetChildrenCount(); i++) {
        updateNodeTransform(child->GetChildren()[i], abs);
    }
}

void AnimatorComponent::OnPreRender() {
    Mat4 identity_matrix = Mat4(1);
    updateNodeTransform(GetEntity(), identity_matrix);
}