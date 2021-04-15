#pragma once

#include "Entity.hpp"

namespace VSGE {
	class Scene {
	private:
		Entity* mRootEntity;

	public:
		
		void NewScene();
		/// <summary>
		/// Create and add new entity to scene and assign name "Entity"
		/// </summary>
		/// <returns></returns>
		Entity* AddNewEntity();
		/// <summary>
		/// Create and add new entity to scene and assign its name
		/// </summary>
		/// <param name="name">name to set for new entity</param>
		/// <returns>pointer to new entity</returns>
		Entity* AddNewEntity(std::string name);
		/// <summary>
		/// Returns pointer to root entity of scene
		/// </summary>
		/// <returns>pointer to root entity</returns>
		Entity* GetRootEntity() { return mRootEntity; }
		/// <summary>
		/// Tries to find the entity with the given name in this entity hierarchy
		/// </summary>
		/// <param name="name"></param>
		/// <returns>pointer to entity</returns>
		Entity* GetEntityWithName(std::string name);
		/// <summary>
		/// Tries to find the entity with the given Guid in this entity hierarchy
		/// </summary>
		/// <param name="id">- guid to find</param>
		/// <returns>pointer to entity</returns>
		Entity* GetEntityWithGuid(const Guid& id);
		
		Scene() : mRootEntity(nullptr) {}
	};
}