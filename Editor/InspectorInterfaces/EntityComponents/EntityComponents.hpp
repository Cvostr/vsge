#pragma once

#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>
#include <Scene/EntityComponents/AudioSourceComponent.hpp>
#include <Scene/EntityComponents/RigidBodyComponent.hpp>
#include <Scene/EntityComponents/ColliderComponent.hpp>
#include <Scene/EntityComponents/EntityScriptComponent.hpp>
#include <Scene/Camera.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>

namespace VSGEditor {
	void DrawLightsourceComponent(VSGE::LightsourceComponent* light);
	void DrawMeshComponent(VSGE::MeshComponent* light);
	void DrawMaterialComponent(VSGE::MaterialComponent* mat);
	void DrawParticleEmitterComponent(VSGE::ParticleEmitterComponent* pec);
	void DrawAnimatorComponent(VSGE::AnimatorComponent* anic);
	void DrawAudioSourceComponent(VSGE::AudioSourceComponent* asc);
	void DrawRigidbodyComponent(VSGE::RigidBodyComponent* rgbc);
	void DrawColliderComponent(VSGE::ColliderComponent* clc);
	void DrawScriptComponent(VSGE::EntityScriptComponent* esc);
	void DrawCameraComponent(VSGE::Camera* cac);
	void DrawTerrainComponent(VSGE::TerrainComponent* tc);
}