#pragma once

#include "Entity.hpp"
#include "EntityComponents/AudioListenerComponent.hpp"
#include "SceneEnvironment.hpp"
#include <map>

namespace VSGE {

	typedef std::pair<Guid, Guid> tGuidPair;

	class Scene {
	private:
		Entity* _rootEntity;
		SceneEnvironmentSettings _environment_settings;
		std::map<AABB, std::vector<Entity*>> _scene_tree;

		bool _running;
		bool _paused;

		void CallOnStart(Entity* entity);
		void CallOnUpdate(Entity* entity);
		void CallOnStop(Entity* entity);
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
		Entity* GetEntityWithName(const std::string& name) const;

		std::vector<Entity*> GetAllEntitiesWithName(const std::string& name) const;
		/// <summary>
		/// Tries to find the entity with the given Guid in this entity hierarchy
		/// </summary>
		/// <param name="id">- guid to find</param>
		/// <returns>pointer to entity</returns>
		Entity* GetEntityWithGuid(const Guid& id);
		/// <summary>
		/// Add objects from binary prefab
		/// </summary>
		/// <param name="data">- prefab data</param>
		/// <param name="size">- prefab data size</param>
		void AddFromPrefab(byte* data, uint32 size);

		AudioListenerComponent* GetAudioListener(Entity* ent = nullptr);
		
		SceneEnvironmentSettings& GetEnvironmentSettings();
		/// <summary>
		/// Get all scene AABB
		/// </summary>
		/// <returns></returns>
		AABB GetSceneBoundingBox();

		void Run();

		void Pause();

		void Stop();

		void Update();

		bool IsSceneRunning();

		bool IsScenePaused();

		void GetEntitiesIntersects(const AABB& bb, std::vector<Entity*>& array, Entity* first = nullptr);

		void UpdateSceneTree(const Vec3& size = Vec3(100, 50, 100));

		Scene();
		~Scene();
	};
}