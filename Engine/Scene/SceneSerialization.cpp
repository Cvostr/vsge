#include "SceneSerialization.hpp"
#include <Core/YamlHelper.hpp>
#include <fstream>

using namespace VSGE;
using namespace YAML;

void SceneSerializer::SerializeEntityComponent(IEntityComponent* component, YAML::Emitter& e) {
	e << BeginMap;
	e << Key << "Component" << Value << (int)component->GetType();

	component->Serialize(e);

	e << YAML::EndMap; // Entity
}

void SceneSerializer::DeserializeEntityComponent(Entity* ent, YAML::Node& comp) {
	int component_id = comp["Component"].as<int>();

	IEntityComponent* component = CreateEntityComponent((EntityComponentType)component_id);
	
	if (component == nullptr)
		return;

	component->SetEntity(ent);
	component->Deserialize(comp);

	ent->AddComponent(component);
}

void SceneSerializer::SerializeEntity(Entity* ent, Emitter& e) {
	if (ent->GetParent()) {
		e << BeginMap;
		e << Key << "Entity" << Value << ent->GetName();
		e << Key << "uid" << Value << ent->GetGuid();
		e << Key << "active" << Value << ent->IsActive();
		e << Key << "static" << Value << ent->IsStatic();
		e << Key << "view_mask" << Value << ent->GetViewMask();
		e << Key << "parent" << Value << ent->GetParent()->GetGuid();
		//write transform
		e << Key << "pos" << Value << ent->GetPosition();
		e << Key << "scale" << Value << ent->GetScale();
		e << Key << "rot" << Value << ent->GetRotation();
		//write scripts
		e << YAML::Key << "components" << YAML::Value << YAML::BeginSeq;
		for (uint32 comp_i = 0; comp_i < ent->GetComponentsCount(); comp_i++) {
			SerializeEntityComponent(ent->GetComponents()[comp_i], e);
		}
		e << YAML::EndSeq;
		//write scripts
		e << YAML::Key << "script" << YAML::Value << YAML::BeginSeq;
		for (uint32 script_i = 0; script_i < ent->GetScriptsCount(); script_i++) {
			e << BeginMap;
			e << Key << "class" << Value << ent->GetScripts()[script_i]->GetClassName();
			e << EndMap;
		}
		e << YAML::EndSeq;

		e << YAML::EndMap; // Entity
	}
	for (uint32 child_i = 0; child_i < ent->GetChildrenCount(); child_i++) {
		SerializeEntity(ent->GetChildren()[child_i], e);
	}
}

void SceneSerializer::Serialize(const std::string& path) {
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	//Store environment settings
	SceneEnvironmentSettings& settings = _scene->GetEnvironmentSettings();
	out << YAML::Key << "env_amb_color" << YAML::Value << settings.GetAmbientColor();
	out << YAML::Key << "env_skybox" << YAML::Value << settings._skybox_material.GetResourceName();

	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	
	SerializeEntity(_scene->GetRootEntity(), out);

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(path, std::ios_base::binary);
	fout.write(out.c_str(), out.size());
	fout.close();
}

void SceneSerializer::DeserializeEntity(Entity* ent, Node& entity) {
	ent->SetScene(_scene);
	std::string name = entity["Entity"].as<std::string>(); 
	ent->SetName(name);
	ent->SetGuid(entity["uid"].as<Guid>());
	ent->SetActive(entity["active"].as<bool>());
	ent->SetStatic(entity["static"].as<bool>());
	ent->SetViewMask(entity["view_mask"].as<ViewMask>());
	//read transform
	ent->SetPosition(entity["pos"].as<Vec3>());
	ent->SetScale(entity["scale"].as<Vec3>());
	ent->SetRotation(entity["rot"].as<Quat>());
	//read components
	YAML::Node components = entity["components"];
	for(auto component : components) {
		DeserializeEntityComponent(ent, component);
	}
	//read scripts
	YAML::Node scripts = entity["script"];
	for (auto script : scripts) {
		EntityScriptComponent* script_ptr = new EntityScriptComponent;
		script_ptr->SetClassName(script["class"].as<std::string>());
		ent->AddScript(script_ptr);
	}

	Guid parent = entity["parent"].as<Guid>();
	Entity* parent_ent = _scene->GetEntityWithGuid(parent);
	//Add child to parent without retransforming
	parent_ent->AddChild(ent, false);
}

bool SceneSerializer::Deserialize(const std::string& path) {
	Node data = YAML::LoadFile(path);
	if (!data["Scene"])
		return false;

	//Load environment settings
	SceneEnvironmentSettings& settings = _scene->GetEnvironmentSettings();
	settings.SetAmbientColor(data["env_amb_color"].as<Color>());
	settings._skybox_material.SetResource(data["env_skybox"].as<std::string>());

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			Entity* ent = new Entity;
			DeserializeEntity(ent, entity);
		}
	}

	return true;
}

bool SceneSerializer::Deserialize(byte* scene_data) {
	Node data = YAML::Load((const char*)scene_data);
	if (!data["Scene"])
		return false;

	//Load environment settings
	SceneEnvironmentSettings& settings = _scene->GetEnvironmentSettings();
	settings.SetAmbientColor(data["env_amb_color"].as<Color>());
	settings._skybox_material.SetResource(data["env_skybox"].as<std::string>());

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			Entity* ent = new Entity;
			DeserializeEntity(ent, entity);
		}
	}

	return true;
}