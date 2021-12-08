#include "SceneSerialization.hpp"
#include <string>
#include <Core/Memory.hpp>

using namespace VSGE;

void SceneSerializer::SerializeBinary(const std::string& path) {
	byte* data = nullptr;
	uint32 size = 0;

	SerializeBinary(&data, size);
}

void SceneSerializer::SerializeBinary(byte** data, uint32& size) {
	ByteSerialize serializer;

	serializer.WriteBytes("vsgebscene", 11);
	//Store environment settings
	SceneEnvironmentSettings& settings = _scene->GetEnvironmentSettings();
	serializer.Serialize(settings.GetAmbientColor());
	serializer.Serialize(settings._skybox_material.GetResourceName());
	
	uint32 entities_count = _scene->GetEntitiesCount() + 1;
	serializer.Serialize(entities_count);

	SerializeEntityBinary(_scene->GetRootEntity(), serializer);

	size = serializer.GetSerializedSize();
	*data = new byte[size];
	Memcpy(*data, serializer.GetBytes(), size);
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
	uint32 scripts_count = ent->GetScriptsCount();
	uint32 children_count = ent->GetChildrenCount();
	serializer.Serialize(components_count);
	serializer.Serialize(scripts_count);
	//Serialize components
	for (uint32 comp_i = 0; comp_i < ent->GetComponentsCount(); comp_i++) {
		SerializeEntityComponentBinary(ent->GetComponents()[comp_i], serializer);
	}
	//serialize scripts
	for (uint32 script_i = 0; script_i < scripts_count; script_i++) {
		ent->GetScripts()[script_i]->Serialize(serializer);
	}
	//Serialize children
	for (uint32 child_i = 0; child_i < ent->GetChildrenCount(); child_i++) {
		SerializeEntityBinary(ent->GetChildren()[child_i], serializer);
	}
}

void SceneSerializer::SerializeEntityComponentBinary(IEntityComponent* component, ByteSerialize& serializer) {
	EntityComponentType type = component->GetType();
	serializer.Serialize(type);
	component->Serialize(serializer);
}

//DESERIALIZE

bool SceneSerializer::DeserializeBinary(byte* data, uint32 size) {
	ByteSolver solver(data, size);

	std::string header = solver.ReadNextString();

	if (header != "vsgebscene")
		return false;

	SceneEnvironmentSettings& settings = _scene->GetEnvironmentSettings();
	settings.SetAmbientColor(solver.GetValue<Color>());
	settings._skybox_material.SetResource(solver.ReadNextString());

	uint32 entity_count = solver.GetValue<uint32>();

	for (uint32 entity_i = 0; entity_i < entity_count; entity_i++) {
		Entity* ent = new Entity;
		DeserializeEntityBinary(ent, solver);
		if (entity_i == 0) {
			_scene->GetRootEntity()->RemoveChild(ent);
			delete ent;
		}
	}

	return true;
}

void SceneSerializer::DeserializeEntityBinary(Entity* ent, ByteSolver& solver, Guid* parent_id) {
	ent->SetScene(_scene);
	ent->SetName(solver.ReadNextString());
	ent->SetGuid(solver.GetValue<Guid>());
	ent->SetActive(solver.GetValue<bool>());
	ent->SetStatic(solver.GetValue<bool>());
	ent->SetViewMask(solver.GetValue<ViewMask>());
	Guid parent = solver.GetValue<Guid>();
	ent->SetPosition(solver.GetValue<Vec3>());
	ent->SetScale(solver.GetValue<Vec3>());
	ent->SetRotation(solver.GetValue<Quat>());

	uint32 components_count = solver.GetValue<uint32>();
	uint32 scripts_count = solver.GetValue<uint32>();
	//Deserialize components
	for (uint32 comp_i = 0; comp_i < components_count; comp_i++) {
		DeserializeEntityComponentBinary(ent, solver);
	}
	for (uint32 script_i = 0; script_i < scripts_count; script_i++) {
		EntityScriptComponent* script_ptr = new EntityScriptComponent;
		script_ptr->Deserialize(solver);
		ent->AddScript(script_ptr);
	}

	if (parent_id != nullptr) {
		*parent_id = parent;
	}
	//Find parent
	Entity* parent_ent = _scene->GetEntityWithGuid(parent);
	//Add child to parent without retransforming
	if(parent_ent != nullptr)
		parent_ent->AddChild(ent, false);
}

void SceneSerializer::DeserializeEntityComponentBinary(Entity* ent, ByteSolver& solver) {
	EntityComponentType component_id = solver.GetValue<EntityComponentType>();

	IEntityComponent* component = CreateEntityComponent(component_id);
	
	if (component == nullptr)
		return;

	component->SetEntity(ent);
	component->Deserialize(solver);

	ent->AddComponent(component);
}
