#include "Renderer.hpp"

#include <Math/MatrixTransform.hpp>

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>

using namespace VSGE;

void IRenderer::CreateRenderList() {
	_entitiesToRender.clear();
	_lightsources.clear();
	_particleEmitters.clear();
	_cameras.clear();
	if(mScene)
		ProcessEntity(mScene->GetRootEntity());
}
void IRenderer::ProcessEntity(Entity* entity) {
	if (!entity->IsActive())
		return;
	
	entity->UpdateTransformMatrices();

	//Call OnPreRender on entity
	entity->CallOnPreRender();

	bool HasMesh = entity->HasComponent<MeshComponent>();
	bool HasMaterial = entity->HasComponent<MaterialComponent>();
	bool hasLightsource = entity->HasComponent<LightsourceComponent>();
	bool hasParticleEmitter = entity->HasComponent<ParticleEmitterComponent>();
	bool hasCamera = entity->HasComponent<Camera>();
	if (HasMesh) {
		MeshComponent* mesh = entity->GetComponent<MeshComponent>();
		if (mesh->GetMeshResource()) {
			
			mesh->GetMeshResource()->Load();
			HasMesh = true;
		}
		else
			HasMesh = false;
	}

	if (HasMaterial) {
		MaterialComponent* mat = entity->GetComponent<MaterialComponent>();
		if (mat->GetMaterialResource()) {
			mat->GetMaterialResource()->Load();
			HasMaterial = true;
		}
		else
			HasMaterial = false;
	}

	if (HasMaterial && HasMesh) {
		_entitiesToRender.push_back(entity);
	}

	if (hasLightsource) {
		_lightsources.push_back(entity);
	}

	if (hasParticleEmitter) {
		_particleEmitters.push_back(entity);
	}

	if (hasCamera) {
		_cameras.push_back(entity);
	}

	for (uint32 child_i = 0; child_i < entity->GetChildrenCount(); child_i ++) {
		Entity* child = entity->GetChildren()[child_i];
		ProcessEntity(child);
	}
}