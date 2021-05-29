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

Vec3 ToEulerAngles(Quat q) {
	const float check = 2.0f * (-q.y * q.z + q.w * q.x);
	float RAD_TO_DEG = 180.f / 3.14159265f;

	if (check < -0.995f)
	{
		return Vec3
		(
			-90.0f,
			0.0f,
			-atan2f(2.0f * (q.x * q.z - q.w * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)) * RAD_TO_DEG
		);
	}

	if (check > 0.995f)
	{
		return Vec3
		(
			90.0f,
			0.0f,
			atan2f(2.0f * (q.x * q.z - q.w * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)) * RAD_TO_DEG
		);
	}

	return Vec3
	(
		asinf(check) * RAD_TO_DEG,
		atan2f(2.0f * (q.x * q.z + q.w * q.y), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)) * RAD_TO_DEG,
		atan2f(2.0f * (q.x * q.y + q.w * q.z), 1.0f - 2.0f * (q.x * q.x + q.z * q.z)) * RAD_TO_DEG
	);
}

Entity* VSGEditor::addObjectFromNode(Scene* w, SceneNode* node) {
	Entity* result = w->AddNewEntity(node->GetLabel());

	//set node transform to object transform
	result->SetPosition(node->GetTranslation());
	result->SetScale(node->GetScale());
	Quat node_rotation = node->GetRotation();
	//Vec3 rot = ToEulerAngles(node_rotation);
	result->SetRotation(node->GetNodeTransform().GetRotation());
	
	//result->SetRotation(Vec3(node_rotation.GetRoll(), node_rotation.GetPitch(), node_rotation.GetYaw()));

	//Mat4 node_tr = GetTranslationMatrix(result->GetPosition()) * GetScaleMatrix(result->GetScale()) * GetRotationMatrix(node_rotation);
	//Mat4 node_tr_s = GetTransform(result->GetPosition(), result->GetScale(), result->GetRotation());

	Mat4 quat_mat = GetRotationMatrix(node_rotation);
	//result->SetRotation(quat_mat.GetRotation());

	Mat4 node_tr = GetTranslationMatrix(result->GetPosition()) * GetScaleMatrix(result->GetScale()) * GetRotationMatrix(node_rotation);
	Mat4 node_tr_s = GetTransform(result->GetPosition(), result->GetScale(), result->GetRotation());

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
		MeshComponent* meshc = object->AddComponent<MeshComponent>();
		meshc->SetMeshName(mesh_label);
		object->AddComponent<MaterialComponent>();
		result->AddChild(object);
	}
	return result;
}