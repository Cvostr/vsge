#pragma once

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>
#include <Core/ByteSerialize.hpp>
#include <Core/ByteSolver.hpp>
#include "Entity.hpp"

namespace VSGE {
	class SceneSerializer {
	private:
		Scene* _scene;
	public:
		void SetScene(Scene* scn) {
			_scene = scn;
		}

		void SerializeBinary(byte** data, uint32& size);
		void SerializeBinary(const std::string& path);
		void Serialize(const std::string& path);

		void SerializeEntity(Entity* ent, YAML::Emitter& e);
		void SerializeEntityBinary(Entity* ent, ByteSerialize& serializer);
		
		void SerializeEntityComponent(IEntityComponent* component, YAML::Emitter& e);
		void SerializeEntityComponentBinary(IEntityComponent* component, ByteSerialize& serializer);

		///DESERIALIZE

		void DeserializeEntityComponent(Entity* ent, YAML::Node& comp);
		void DeserializeEntityComponentBinary(Entity* ent, ByteSolver& solver);

		void DeserializeEntity(Entity* ent, YAML::Node& entity);
		void DeserializeEntityBinary(Entity* ent, ByteSolver& solver, Guid* parent_id = nullptr);

		bool Deserialize(const std::string& path);
		bool Deserialize(byte* scene_data);
		bool DeserializeBinary(byte* data, uint32 size);
	};
}