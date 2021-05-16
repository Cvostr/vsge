#pragma once

#include <vector>
#include <Core/Serialization/ISerializable.hpp>

namespace VSGE {

	class Entity;

	class IEntityComponent : public ISerializable {
	public:

		IEntityComponent() : mActive(false), mEntity(nullptr) {}

		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}

		/// <summary>
		/// Set active/inactive state of entity
		/// </summary>
		/// <param name="active"></param>
		void SetActive(bool active) { mActive = active; }
		/// <summary>
		/// Return whether component is active.
		/// </summary>
		/// <returns></returns>
		bool IsActive() { return mActive; }
		/// <summary>
		/// Store pointer to entity, that contains this component
		/// </summary>
		/// <param name="entity">- pointer to entity</param>
		void SetEntity(Entity* entity) { mEntity = entity; }
	private:
		bool mActive;
		Entity* mEntity;
	};

	typedef std::vector<IEntityComponent*> tComponentList;
}