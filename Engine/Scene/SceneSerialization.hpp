#pragma once

#include "Scene.hpp"
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

		void SerializeEntityBinary(Entity* ent, ByteSerialize& serializer);
		void SerializeEntityComponentBinary(IEntityComponent* component, ByteSerialize& serializer);

		///DESERIALIZE

		void DeserializeEntityComponentBinary(Entity* ent, ByteSolver& solver);
		void DeserializeEntityBinary(Entity* ent, ByteSolver& solver, Guid* parent_id = nullptr);

		bool DeserializeBinary(const std::string& path);
		bool DeserializeBinary(byte* data, uint32 size);
	};
}