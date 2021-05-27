#pragma once

#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>

namespace VSGEditor {
	void DrawLightsourceComponent(VSGE::LightsourceComponent* light);
	void DrawMeshComponent(VSGE::MeshComponent* light);
	void DrawMaterialComponent(VSGE::MaterialComponent* mat);
	void DrawParticleEmitterComponent(VSGE::ParticleEmitterComponent* pec);
}