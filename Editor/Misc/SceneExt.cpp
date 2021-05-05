#include "SceneExt.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>

#include <Math/MatrixTransform.hpp>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::AddSubSceneVS3M(Scene* scn, const std::string& file) {
	ImportedSceneFile isf;
	isf.loadFromFile(file);
	SceneNode* node = isf.rootNode;

	addObjectFromNode(scn, node);
	isf.clearMeshes();
}

Entity* VSGEditor::addObjectFromNode(Scene* w, SceneNode* node) {
	Entity* result = w->AddNewEntity(node->GetLabel());

	//set node transform to object transform
	result->SetPosition(node->GetTranslation());
	result->SetScale(node->GetScale());
	Quat node_rotation = node->GetRotation();
	result->SetRotation(Vec3(node_rotation.GetPitch(), node_rotation.GetYaw(), node_rotation.GetRoll()) * 3.14159f / 180.f);

	//Mat4 node_tr = GetTranslationMatrix(result->GetPosition()) * GetScaleMatrix(result->GetScale()) * GetRotationMatrix(node_rotation);
	//Mat4 node_tr = GetTransform(result->GetPosition(), result->GetScale(), result->GetRotation());

	//Iterate over all children nodes
	for (uint32 node_i = 0; node_i < node->children.size(); node_i++) {
		SceneNode* ptr = node->children[node_i];
		//Create new node object
		Entity* newobj = addObjectFromNode(w, ptr);
		result->AddChild(newobj);
	}
	//Iterate over all children meshes
	for (uint32 node_i = 0; node_i < node->mesh_names.size(); node_i++) {
		std::string mesh_label = node->mesh_names[node_i];
		Entity* object = w->AddNewEntity(mesh_label);
		object->AddComponent<MeshComponent>();
		object->AddComponent<MaterialComponent>();
		//strcpy(object->meshInfo.mesh_label, mesh_label.c_str());
		result->AddChild(object);
	}
	return result;
}