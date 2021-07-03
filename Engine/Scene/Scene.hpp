#pragma once

#include <Math/Color.hpp>
#include "Entity.hpp"
#include "EntityComponents/AudioListenerComponent.hpp"

namespace VSGE {

	class SceneSettings {
		Color _ambient_color;
		Entity* _sun;
	};

	class Scene {
	private:
		Entity* _rootEntity;

	public:
		/// <summary>
		/// Clear all objects and setup new empty scene
		/// </summary>
		void NewScene();
		/// <summary>
		/// Create and add new entity to scene and assign its name
		/// </summary>
		/// <param name="name">name to set for new entity</param>
		/// <returns>pointer to new entity</returns>
		Entity* AddNewEntity(const std::string& name);
		/// <summary>
		/// Returns pointer to root entity of scene
		/// </summary>
		/// <returns>pointer to root entity</returns>
		Entity* GetRootEntity();
		/// <summary>
		/// Get count of entities in scene
		/// </summary>
		/// <returns></returns>
		uint32 GetEntitiesCount();
		/// <summary>
		/// Tries to find the entity with the given name in this entity hierarchy
		/// </summary>
		/// <param name="name"></param>
		/// <returns>pointer to entity</returns>
		Entity* GetEntityWithName(const std::string& name);
		/// <summary>
		/// Tries to find the entity with the given Guid in this entity hierarchy
		/// </summary>
		/// <param name="id">- guid to find</param>
		/// <returns>pointer to entity</returns>
		Entity* GetEntityWithGuid(const Guid& id);

		void AddFromPrefab(byte* data, uint32 size);

		AudioListenerComponent* GetAudioListener(Entity* ent = nullptr);

		Scene();
	};
}