#include "SceneSerialization.hpp"
#include <fstream>


using namespace VSGE;
using namespace YAML;

namespace YAML {

	template<>
	struct convert<VSGE::Guid>
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
	struct convert<Vec3>
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
}

Emitter& operator<<(YAML::Emitter& out, const Guid& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.a << v.b << v.c << v.d << YAML::EndSeq;
	return out;
}

Emitter& operator<<(YAML::Emitter& out, const Vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

void SceneSerializer::SerializeEntity(Entity* ent, Emitter& e) {
	if (ent->GetParent()) {
		e << BeginMap;
		e << Key << "Entity" << Value << ent->GetName();
		e << Key << "uid" << Value << ent->GetGuid();
		e << Key << "active" << Value << ent->IsActive();
		e << Key << "static" << Value << ent->IsStatic();
		e << Key << "parent" << Value << ent->GetParent()->GetGuid();

		e << Key << "pos" << Value << ent->GetPosition();
		e << Key << "scale" << Value << ent->GetScale();
		e << Key << "rot" << Value << ent->GetRotation();

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
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	
	SerializeEntity(_scene->GetRootEntity(), out);

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
}

void SceneSerializer::DeserializeEntity(Entity* ent, Node& entity) {
	ent->SetScene(_scene);
	std::string name = entity["Entity"].as<std::string>(); // TODO
	ent->SetName(name);
	ent->SetGuid(entity["uid"].as<Guid>());
	ent->SetActive(entity["active"].as<bool>());
	ent->SetStatic(entity["static"].as<bool>());

	ent->SetPosition(entity["pos"].as<Vec3>());
	ent->SetScale(entity["scale"].as<Vec3>());
	ent->SetRotation(entity["rot"].as<Vec3>());

	Guid parent = entity["parent"].as<Guid>();
	Entity* parent_ent = _scene->GetEntityWithGuid(parent);
	parent_ent->AddChild(ent);
}

bool SceneSerializer::Deserialize(const std::string& path) {
	Node data = YAML::LoadFile(path);
	if (!data["Scene"])
		return false;

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			Entity* ent = new Entity;
			DeserializeEntity(ent, entity);
		}
	}
}