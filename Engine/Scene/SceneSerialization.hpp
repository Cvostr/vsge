#pragma once

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>
#include "Entity.hpp"

namespace VSGE {
	class SceneSerializer {
	private:
		Scene* _scene;
	public:
		void SetScene(Scene* scn) {
			_scene = scn;
		}

		void Serialize(const std::string& path);
		void SerializeEntity(Entity* ent, YAML::Emitter& e);
		void SerializeEntityComponent(IEntityComponent* component, YAML::Emitter& e);

		void DeserializeEntityComponent(Entity* ent, YAML::Node& comp);
		void DeserializeEntity(Entity* ent, YAML::Node& entity);
		bool Deserialize(const std::string& path);
	};
}