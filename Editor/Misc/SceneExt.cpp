#include "SceneExt.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>

#include <Math/MatrixTransform.hpp>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::AddSubSceneVS3M(Scene* scn, const std::string& file) {
	ImportedSceneFile isf;
	isf.loadFromFile(file);
	SceneNode* node = isf.rootNode;

	Entity* rootEntity = addObjectFromNode(scn, node);
	rootEntity->AddComponent<AnimatorComponent>();

	isf.clearMeshes();
}

Entity* VSGEditor::addObjectFromNode(Scene* w, SceneNode* node) {
	Entity* result = w->AddNewEntity(node->GetLabel());

	//set node transform to object transform
	Mat4 node_transform_transposed = node->GetNodeTransform().transpose();
	Vec3 pos = node_transform_transposed.GetPosition();
	Vec3 sca = node_transform_transposed.GetScale();
	Quat rot = GetRotationFromQuat(node_transform_transposed);

	result->SetPosition(pos);
	result->SetScale(sca);
	result->SetRotation(rot.Conjugate());

	result->SetPosition(node->GetTranslation());
	result->SetScale(node->GetScale());
	Quat node_rotation = node->GetRotation().Conjugate();
	result->SetRotation(node_rotation);
	

	//Iterate over all children nodes
	for (uint32 node_i = 0; node_i < node->children.size(); node_i++) {
		SceneNode* ptr = node->children[node_i];
		//Create new node object
		Entity* newobj = addObjectFromNode(w, ptr);
		result->AddChild(newobj, false);
	}
	if (node->mesh_names.size() == 1) {
		MeshComponent* meshc = result->AddComponent<MeshComponent>();
		meshc->SetMeshName(node->mesh_names[0]);
		MaterialComponent* mat = result->AddComponent<MaterialComponent>();
		mat->SetMaterialName("Default Material");
	}
	else {
		//Iterate over all children meshes
		for (uint32 node_i = 0; node_i < node->mesh_names.size(); node_i++) {
			std::string mesh_label = node->mesh_names[node_i];
			Entity* object = w->AddNewEntity(mesh_label);
			MeshComponent* meshc = object->AddComponent<MeshComponent>();
			meshc->SetMeshName(mesh_label);
			MaterialComponent* mat = object->AddComponent<MaterialComponent>();
			mat->SetMaterialName("Default Material");
			result->AddChild(object, false);
		}
	}
	return result;
}