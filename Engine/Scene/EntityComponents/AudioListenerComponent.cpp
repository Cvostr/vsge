#include "AudioListenerComponent.hpp"
#include <Scene/Entity.hpp>
#include <Audio/AudioLayer.hpp>
#include "../Camera.hpp"

using namespace VSGE;

void AudioListenerComponent::OnUpdate() {
	Camera* cam = _entity->GetComponent<Camera>();
	if (!cam)
		return;
	Vec3 front = cam->GetFront();
	Vec3 up = cam->GetFront();

	AudioLayer* audio = AudioLayer::Get();

	if (!audio)
		return;

	audio->SetListenerPos(cam->GetPosition());
	audio->SetListenerOrientation(front, up);
}