#include "Renderer.hpp"

#include <Math/MatrixTransform.hpp>

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>

using namespace VSGE;

void IRenderer::CreateRenderList() {
	mEntitiesToRender.clear();

	ProcessEntity(mScene->GetRootEntity());
}
void IRenderer::ProcessEntity(Entity* entity) {
	if (!entity->IsActive())
		return;
	Mat4 localTransform = GetTransform(entity->GetPosition(), entity->GetScale(), entity->GetRotation());

	entity->SetLocalTransform(localTransform);

	Vec3 tr = entity->GetPosition();
	Vec3 sc = entity->GetScale();
	Vec3 rt = entity->GetRotation();

	Mat4 worldTransform(1.f);
	if (entity->GetParent()) {
		Mat4 parent = entity->GetParent()->GetWorldTransform();
		tr += parent.GetPosition();
		sc *= parent.GetScale();
		rt += parent.GetRotation();
	}

	worldTransform = GetTransform(tr, sc, rt);
	

	entity->SetWorldTransform(worldTransform);

	bool HasMesh = entity->HasComponent<MeshComponent>();
	bool HasMaterial = entity->HasComponent<MaterialComponent>();
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

	for (uint32 child_i = 0; child_i < entity->GetChildrenCount(); child_i ++) {
		Entity* child = entity->GetChildren()[child_i];
		ProcessEntity(child);
	}
}