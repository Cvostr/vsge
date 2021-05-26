#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

namespace VSGE {

	enum EntityComponentType {
		ENTITY_COMPONENT_NONE,
		ENTITY_COMPONENT_LIGHTSOURCE,
		ENTITY_COMPONENT_MATERIAL,
		ENTITY_COMPONENT_MESH,
		ENTITY_COMPONENT_ANIMATOR,
		ENTITY_COMPONENT_PARTICLE_EMITTER
	};

	class Entity;

	#define DEFINE_ENTITY_COMPONENT(type_id, string_name) \
		static EntityComponentType GetStaticType() { return type_id; } \
		virtual EntityComponentType GetType() const { return GetStaticType(); } \
		static std::string GetTypeStringStatic() { return string_name; } \
		virtual std::string GetTypeString() const { return GetTypeStringStatic(); } \

	class IEntityComponent {
	public:

		IEntityComponent() : _active(false), _entity(nullptr) {}

		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}

		virtual void Serialize(YAML::Emitter& e) {}
		virtual void Deserialize(YAML::Node& entity) {}

		virtual EntityComponentType GetType() const = 0;
		virtual std::string GetTypeString() const = 0;

		/// <summary>
		/// Set active/inactive state of entity
		/// </summary>
		/// <param name="active"></param>
		void SetActive(bool active) { _active = active; }
		/// <summary>
		/// Return whether component is active.
		/// </summary>
		/// <returns></returns>
		bool IsActive() const { return _active; }
		/// <summary>
		/// Store pointer to entity, that contains this component
		/// </summary>
		/// <param name="entity">- pointer to entity</param>
		void SetEntity(Entity* entity) { _entity = entity; }

		Entity* GetEntity() const {
			return _entity;
		}
	private:
		bool _active;
		Entity* _entity;
	};

	typedef std::vector<IEntityComponent*> tComponentList;
}