#pragma once

#include "Scene.hpp"
#include <Core/ByteSerialize.hpp>
#include <Core/ByteSolver.hpp>
#include "Entity.hpp"

namespace VSGE {
	class SceneSerializer {
	private:
		Scene* _scene;
		Scene* _permanentScene;
	public:
		
		SceneSerializer();

		void SetScene(Scene* scn);
		void SetPermanentScene(Scene* scn);

		void SerializeBinary(byte** data, uint32& size);
		void SerializeBinary(const std::string& path);

		void SerializeEntityBinary(Entity* ent, ByteSerialize& serializer);
		void SerializeEntityComponentBinary(IEntityComponent* component, ByteSerialize& serializer);

		///DESERIALIZE

		void DeserializeEntityComponentBinary(Entity* ent, ByteSolver& solver);
		void DeserializeEntityBinary(Entity* ent, ByteSolver& solver, Guid* parent_id = nullptr, bool addToParent = true);

		void DeserializeBinary(const std::string& path);
		void DeserializeBinary(byte* data, uint32 size);
	};
}