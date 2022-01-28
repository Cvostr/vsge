#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/AnimatorComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void Play(AnimatorComponent* anim) {
	anim->Play();
}

static void Stop(AnimatorComponent* anim) {
	anim->Stop();
}

static bool IsPlaying(AnimatorComponent* anim) {
	return anim->IsPlaying();
}

static void AddAnimation(AnimatorComponent* anim) {
	anim->AddNewAnimation();
}

static void SetAnimationCoeff(AnimatorComponent* anim, uint32 index, float coeff) {
	std::vector<AnimationCoeff>& coeffs = anim->GetAnimations();
	coeffs[index].coeff = coeff;
}

static void SetAnimation(AnimatorComponent* anim, uint32 index, Resource* res) {
	std::vector<AnimationCoeff>& coeffs = anim->GetAnimations();
	coeffs[index]._animResource.SetResource(res->GetName());
}

void VSGE::BindAnimator() {
	MonoScriptingLayer::AddInternalCall("AnimatorComponent::i_Play(ulong)", Play);
	MonoScriptingLayer::AddInternalCall("AnimatorComponent::i_Stop(ulong)", Stop);
	MonoScriptingLayer::AddInternalCall("AnimatorComponent::i_IsPlaying(ulong)", IsPlaying);

	MonoScriptingLayer::AddInternalCall("AnimatorComponent::i_AddAnimation(ulong)", AddAnimation);
	MonoScriptingLayer::AddInternalCall("AnimatorComponent::i_SetAnimationCoeff(ulong,single)", SetAnimationCoeff);
	MonoScriptingLayer::AddInternalCall("AnimatorComponent::i_SetAnimation(ulong,uint,string)", SetAnimation);
}