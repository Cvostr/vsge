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
#include <Scene/EntityComponents/CharacterControllerComponent.hpp>

#define TERRAIN_EDITOR_EDIT_MODE_HEIGHT 1
#define TERRAIN_EDITOR_EDIT_MODE_TEXTURES 2
#define TERRAIN_EDITOR_EDIT_MODE_GRASS 3

namespace VSGEditor {
	void DrawLightsourceComponent(VSGE::LightsourceComponent* light);
	void DrawMeshComponent(VSGE::MeshComponent* light);
	void DrawMaterialComponent(VSGE::MaterialComponent* mat);
	void DrawParticleEmitterComponent(VSGE::ParticleEmitterComponent* pec);
	void DrawAnimatorComponent(VSGE::AnimatorComponent* anic);
	void DrawAudioSourceComponent(VSGE::AudioSourceComponent* asc);
	void DrawRigidbodyComponent(VSGE::RigidBodyComponent* rgbc);
	void DrawColliderComponent(VSGE::ColliderComponent* clc);
	void DrawScriptComponent(VSGE::EntityScriptComponent* esc, uint32 index);
	void DrawCameraComponent(VSGE::Camera* cac);
	void DrawCharControllerComponent(VSGE::CharacterControllerComponent* ccc);
}