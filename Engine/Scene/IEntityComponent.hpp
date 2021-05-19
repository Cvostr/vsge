#pragma once

#include <vector>

namespace VSGE {

	enum EntityComponentType {
		ENTITY_COMPONENT_NONE,
		ENTITY_COMPONENT_LIGHTSOURCE,
		ENTITY_COMPONENT_MATERIAL,
		ENTITY_COMPONENT_MESH,
		ENTITY_COMPONENT_ANIMATOR
	};

	class Entity;

	class IEntityComponent {
	public:

		IEntityComponent() : _active(false), _entity(nullptr) {}

		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}
		virtual EntityComponentType GetType() = 0 {
			return ENTITY_COMPONENT_NONE;
		}

		/// <summary>
		/// Set active/inactive state of entity
		/// </summary>
		/// <param name="active"></param>
		void SetActive(bool active) { _active = active; }
		/// <summary>
		/// Return whether component is active.
		/// </summary>
		/// <returns></returns>
		bool IsActive() { return _active; }
		/// <summary>
		/// Store pointer to entity, that contains this component
		/// </summary>
		/// <param name="entity">- pointer to entity</param>
		void SetEntity(Entity* entity) { _entity = entity; }
	private:
		bool _active;
		Entity* _entity;
	};

	typedef std::vector<IEntityComponent*> tComponentList;
}