#include "Renderer.hpp"

#include <Math/MatrixTransform.hpp>

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>

using namespace VSGE;

void IRenderer::CreateRenderList() {
	mEntitiesToRender.clear();

	ProcessEntity(mScene->GetRootEntity());
}
void IRenderer::ProcessEntity(Entity* entity) {
	if (!entity->IsActive())
		return;
	//Calculate local transformation matrix
	Mat4 localTransform = GetTransform(entity->GetPosition(), entity->GetScale(), entity->GetRotation());
	//Set local transform matrix to entity
	entity->SetLocalTransform(localTransform);

	Vec3 tr = entity->GetPosition();
	Vec3 sc = entity->GetScale();
	Quat rt = entity->GetRotation();

	Mat4 worldTransform(1.f);
	if (entity->GetParent()) {
		Mat4 parent = entity->GetParent()->GetWorldTransform();
		tr += parent.GetPosition();
		sc *= parent.GetScale();
		rt *= GetRotation(parent);
	}

	//Calculate absolute transform matrix
	worldTransform = GetTransform(tr, sc, rt);
	
	//Update entity absolute transform matrix
	entity->SetWorldTransform(worldTransform);

	//Call OnPreRender on entity
	entity->CallOnPreRender();

	bool HasMesh = entity->HasComponent<MeshComponent>();
	bool HasMaterial = entity->HasComponent<MaterialComponent>();
	bool hasLightsource = entity->HasComponent<LightsourceComponent>();
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
		mEntitiesToRender.push_back(entity);
	}

	if (hasLightsource) {
		mLightsources.push_back(entity);
	}

	for (uint32 child_i = 0; child_i < entity->GetChildrenCount(); child_i ++) {
		Entity* child = entity->GetChildren()[child_i];
		ProcessEntity(child);
	}
}