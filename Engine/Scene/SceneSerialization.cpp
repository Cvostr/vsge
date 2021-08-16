#include "SceneSerialization.hpp"
#include <Core/YamlHelper.hpp>
#include <fstream>

using namespace VSGE;
using namespace YAML;

	template<>
struct YAML::convert<VSGE::Color>
{
	static Node encode(const Color& rhs)
	{
		Node node;
		node.push_back(rhs.r);
		node.push_back(rhs.g);
		node.push_back(rhs.b);
		node.push_back(rhs.a);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, Color& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.r = node[0].as<float>();
		rhs.g = node[1].as<float>();
		rhs.b = node[2].as<float>();
		rhs.a = node[3].as<float>();
		return true;
	}
};

	template<>
	struct YAML::convert<VSGE::Guid>
	{
		static Node encode(const Guid& rhs)
		{
			Node node;
			node.push_back(rhs.a);
			node.push_back(rhs.b);
			node.push_back(rhs.c);
			node.push_back(rhs.d);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Guid& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.a = node[0].as<uint32>();
			rhs.b = node[1].as<uint32>();
			rhs.c = node[2].as<uint32>();
			rhs.d = node[3].as<uint32>();
			return true;
		}
	};

	template<>
	struct YAML::convert<Vec3>
	{
		static Node encode(const Vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct YAML::convert<Quat>
	{
		static Node encode(const Quat& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Quat& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};


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

		e << Key << "pos" << Value << ent->GetPosition();
		e << Key << "scale" << Value << ent->GetScale();
		e << Key << "rot" << Value << ent->GetRotation();

		e << YAML::Key << "components" << YAML::Value << YAML::BeginSeq;
		for (uint32 comp_i = 0; comp_i < ent->GetComponentsCount(); comp_i++) {
			SerializeEntityComponent(ent->GetComponents()[comp_i], e);
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


	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	
	SerializeEntity(_scene->GetRootEntity(), out);

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
}

void SceneSerializer::DeserializeEntity(Entity* ent, Node& entity) {
	ent->SetScene(_scene);
	std::string name = entity["Entity"].as<std::string>(); 
	ent->SetName(name);
	ent->SetGuid(entity["uid"].as<Guid>());
	ent->SetActive(entity["active"].as<bool>());
	ent->SetStatic(entity["static"].as<bool>());
	ent->SetViewMask(entity["view_mask"].as<ViewMask>());

	ent->SetPosition(entity["pos"].as<Vec3>());
	ent->SetScale(entity["scale"].as<Vec3>());
	ent->SetRotation(entity["rot"].as<Quat>());

	YAML::Node components = entity["components"];
	for(auto component : components) {
		DeserializeEntityComponent(ent, component);
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
