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
	//Mat4 localTransform = GetTransform(entity->GetPosition(), entity->GetScale(), entity->GetRotation());
	Mat4 localTransform(1.f);
	localTransform = Scale(localTransform, entity->GetScale());
	localTransform = localTransform * GetRotationMatrix(entity->GetRotation());
	localTransform = Translate(localTransform, entity->GetPosition());

	entity->SetLocalTransform(localTransform);

	Mat4 worldTransform(1.f);
	if (entity->GetParent()) {
		worldTransform = entity->GetParent()->GetWorldTransform();
	}

	worldTransform = Scale(worldTransform, entity->GetScale());
	worldTransform = worldTransform * GetRotationMatrix(entity->GetRotation());
	worldTransform = Translate(worldTransform, entity->GetPosition());
	

	entity->SetWorldTransform(worldTransform);

	bool HasMesh = entity->HasComponent<MeshComponent>();
	bool HasMaterial = entity->HasComponent<MaterialComponent>();
	if (HasMesh) {
		MeshComponent* mesh = entity->GetComponent<MeshComponent>();
		if (mesh->GetMeshResource()){
			mEntitiesToRender.push_back(entity);
			mesh->GetMeshResource()->Load();
		}
	}

	for (uint32 child_i = 0; child_i < entity->GetChildrenCount(); child_i ++) {
		Entity* child = entity->GetChildren()[child_i];
		ProcessEntity(child);
	}
}