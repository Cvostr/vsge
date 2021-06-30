#pragma once

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>
#include <Core/ByteSerialize.hpp>
#include "Entity.hpp"

namespace VSGE {
	class SceneSerializer {
	private:
		Scene* _scene;
	public:
		void SetScene(Scene* scn) {
			_scene = scn;
		}

		void SerializeBinary(const std::string& path);
		void Serialize(const std::string& path);

		void SerializeEntity(Entity* ent, YAML::Emitter& e);
		void SerializeEntityBinary();
		
		void SerializeEntityComponent(IEntityComponent* component, YAML::Emitter& e);
		void SerializeEntityComponentBinary();

		void DeserializeEntityComponent(Entity* ent, YAML::Node& comp);
		void DeserializeEntity(Entity* ent, YAML::Node& entity);
		bool Deserialize(const std::string& path);
	};
}