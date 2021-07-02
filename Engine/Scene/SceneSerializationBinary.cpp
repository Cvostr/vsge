#include "SceneSerialization.hpp"
#include <string>

using namespace VSGE;

void SceneSerializer::SerializeBinary(const std::string& path) {
	ByteSerialize serializer;

	serializer.WriteBytes("vsgebscene", 11);

	uint32 entities_count = _scene->GetEntitiesCount();
	serializer.Serialize(entities_count);
	
	SerializeEntityBinary(_scene->GetRootEntity(), serializer);
}

void SceneSerializer::SerializeEntityBinary(Entity* ent, ByteSerialize& serializer) {
	serializer.Serialize(ent->GetName());
	serializer.Serialize(ent->GetGuid());
	serializer.Serialize(ent->IsActive());
	serializer.Serialize(ent->IsStatic());
	serializer.Serialize(ent->GetViewMask());
	//Write parent guid
	if(ent->GetParent())
		serializer.Serialize(ent->GetParent()->GetGuid());
	else
		serializer.Serialize(ent->GetGuid());

	serializer.Serialize(ent->GetPosition());
	serializer.Serialize(ent->GetScale());
	serializer.Serialize(ent->GetRotation());
	uint32 components_count = ent->GetComponentsCount();
	uint32 children_count = ent->GetChildrenCount();
	serializer.Serialize(children_count);
	serializer.Serialize(components_count);

	for (uint32 comp_i = 0; comp_i < ent->GetComponentsCount(); comp_i++) {
		SerializeEntityComponentBinary(ent->GetComponents()[comp_i], serializer);
	}

	for (uint32 child_i = 0; child_i < ent->GetChildrenCount(); child_i++) {
		SerializeEntityBinary(ent->GetChildren()[child_i], serializer);
	}
}

void SceneSerializer::SerializeEntityComponentBinary(IEntityComponent* component, ByteSerialize& serializer) {
	EntityComponentType type = component->GetType();
	serializer.Serialize(type);
}

//DESERIALIZE

bool SceneSerializer::DeserializeBinary(byte* data, uint32 size) {
	ByteSolver solver(data, size);

	std::string header = solver.ReadNextString();

	if (header != "vsgebscene")
		return false;

	uint32 entity_count = solver.GetValue<uint32>();

	for (uint32 entity_i = 0; entity_i < entity_count; entity_i++) {
		Entity* ent = new Entity;
		DeserializeEntityBinary(ent, solver);
	}

	return true;
}

void SceneSerializer::DeserializeEntityBinary(Entity* ent, ByteSolver& solver) {
	ent->SetName(solver.ReadNextString());
	ent->SetGuid(solver.GetValue<Guid>());
	ent->SetActive(solver.GetValue<bool>());
	ent->SetStatic(solver.GetValue<bool>());
	ent->SetViewMask(solver.GetValue<ViewMask>());
	Guid parent = solver.GetValue<Guid>();
	ent->SetPosition(solver.GetValue<Vec3>());
	ent->SetScale(solver.GetValue<Vec3>());
	ent->SetRotation(solver.GetValue<Quat>());

	uint32 children_count = solver.GetValue<uint32>();
	uint32 components_count = solver.GetValue<uint32>();

	for (uint32 comp_i = 0; comp_i < ent->GetComponentsCount(); comp_i++) {
		//SerializeEntityComponentBinary(ent->GetComponents()[comp_i], serializer);
	}
	for (uint32 child_i = 0; child_i < ent->GetChildrenCount(); child_i++) {
		//SerializeEntityBinary(ent->GetChildren()[child_i], serializer);
	}
}

void SceneSerializer::DeserializeEntityComponentBinary(Entity* ent, ByteSolver& solver) {

}