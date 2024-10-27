#include "LightProbeComponent.hpp"
#include <Engine/Application.hpp>
#include <Graphics/Renderer.hpp>

using namespace VSGE;

LightProbeComponent::LightProbeComponent() {
	image = nullptr;
	baked = false;
	radius = 5.f;

	farPlane = 100.f;
}

LightProbeImage* LightProbeComponent::GetImage() const
{
	return image;
}

void LightProbeComponent::SetImage(LightProbeImage* image)
{
	this->image = image;
}

bool LightProbeComponent::IsBaked() const
{
	return baked;
}

bool LightProbeComponent::LoadImage()
{
	// Пока что только false
	// Это значит что запеченная проба отсутствует и надо создать новую в реалтайме
	return false;
}

float LightProbeComponent::GetRadius() const
{
	return radius;
}

void LightProbeComponent::SetRadius(float radius)
{
	this->radius = radius;
}

void LightProbeComponent::Bake()
{
	LightProbeRenderRequest request = {image, GetEntity()->GetAbsolutePosition(), farPlane};

	Application::Get()->GetRenderer()->EnqueueLightProbe(request);
	baked = true;
}