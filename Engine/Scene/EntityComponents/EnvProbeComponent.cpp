#include "LightProbeComponent.hpp"
#include <Engine/Application.hpp>
#include <Graphics/Renderer.hpp>

using namespace VSGE;

LightProbeComponent::LightProbeComponent() {
	image = nullptr;
	baked = false;
	radius = 5.f;

	nearPlane = 0.01f;
	farPlane = 100.f;
}

LightProbeComponent::~LightProbeComponent()
{
	SAFE_RELEASE(image)
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

float LightProbeComponent::GetNearPlane() const
{
	return nearPlane;
}

void LightProbeComponent::SetNearPlane(float nearPlane)
{
	this->nearPlane = nearPlane;
}

float LightProbeComponent::GetFarPlane() const
{
	return farPlane;
}

void LightProbeComponent::SetFarPlane(float farPlane)
{
	this->farPlane = farPlane;
}

float LightProbeComponent::GetRadius() const
{
	return radius;
}

void LightProbeComponent::SetRadius(float radius)
{
	this->radius = radius;
}

void LightProbeComponent::QueueRender()
{
	LightProbeRenderRequest request = {image, GetEntity()->GetAbsolutePosition(), nearPlane, farPlane};

	Application::Get()->GetRenderer()->EnqueueLightProbe(request);
	baked = true;
}

void LightProbeComponent::Serialize(ByteSerialize& serializer)
{
	serializer.Serialize(nearPlane);
	serializer.Serialize(farPlane);
	serializer.Serialize(radius);
}

void LightProbeComponent::Deserialize(ByteSolver& solver)
{
	nearPlane = solver.GetValue<float>();
	farPlane = solver.GetValue<float>();
	radius = solver.GetValue<float>();
}