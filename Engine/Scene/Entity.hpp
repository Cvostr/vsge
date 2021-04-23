#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/VarTypes/Guid.hpp>
#include "IEntityComponent.hpp"
#include <string>
#include <typeinfo>
#include "Math/Vec3.hpp"
#include "Math/Mat4.hpp"
#include "Math/AABB.hpp"
#include "Camera.hpp"

namespace VSGE {

	class Scene;
	class Entity;

	typedef std::vector<Entity*> tEntityList;

	class Entity {
	private:

		bool mActive;
		bool mStatic;
		ViewMask mViewMask;
		std::string mName;
		Guid mId;

		//Parent of Entity, NULL if root entity
		Entity* mParent; 
		//Scene, that contains this entity
		Scene* mScene; 
		//Array of components, attached to entity
		tComponentList mComponents;
		//Array of children, attached to entity
		tEntityList mChildren; 

		AABB mBoundingBox;
		Vec3 mPosition;
		Vec3 mScale;
		Vec3 mRotation;
		Mat4 LocalTransform;
		bool mTransformDirty;
	public:

		Entity() : mParent(nullptr),
					mScene(nullptr),
					mActive(true),
					mStatic(false),
					mTransformDirty(false),
					mScale(1.f),
					mViewMask(MAX_INT64)
		{}
		/// <summary>
		/// get guid of entity
		/// </summary>
		/// <returns></returns>
		const Guid& GetGuid() { return mId; }
		/// <summary>
		/// Stores pointer to scene in entity class
		/// </summary>
		/// <param name="scene"> - pointer to scene</param>
		void SetScene(Scene* scene) { mScene = scene; }
		/// <summary>
		/// Stores pointer to parent entity in entity class
		/// </summary>
		/// <param name="parent"> - pointer to parent entity</param>
		void SetParent(Entity* parent) { mParent = parent; }
		/// <summary>
		/// Set active/inactive state of entity
		/// </summary>
		/// <param name="active"></param>
		void SetActive(bool active) { mActive = active; }
		/// <summary>
		/// Return whether entity is active.
		/// </summary>
		/// <returns></returns>
		bool IsActive() { return mActive; }

		void SetStatic(bool _static) { mStatic = _static; }
		/// <summary>
		/// Return whether entity is static.
		/// </summary>
		/// <returns></returns>
		bool IsStatic() { return mStatic; }
		/// <summary>
		/// Set new name string to entity
		/// </summary>
		/// <param name="name">- new name string</param>
		void SetName(const std::string& name) { mName = name; }
		/// <summary>
		/// get name of entity
		/// </summary>
		/// <returns></returns>
		const std::string& GetName() { return mName; }
		/// <summary>
		/// Add new child to this entity
		/// </summary>
		/// <param name="entityToAdd">- pointer to new child</param>
		void AddChild(Entity* entityToAdd);
		/// <summary>
		/// Remove child from this entity
		/// </summary>
		/// <param name="entityToRemove">- pointer to child entity to remove</param>
		void RemoveChild(Entity* entityToRemove);
		/// <summary>
		/// Get amount of children, connected to this object
		/// </summary>
		/// <returns></returns>
		uint32 GetChildrenCount() { return static_cast<uint32>(mChildren.size()); }
		/// <summary>
		/// Return child entity by index.
		/// </summary>
		/// <param name="child_index">- index of child to return</param>
		/// <returns></returns>
		Entity* GetChild(uint32 child_index) { return mChildren[child_index]; }
		/// <summary>
		/// Does this entity contain child entity with pointer
		/// </summary>
		/// <param name="entity"></param>
		/// <returns></returns>
		bool HasChild(Entity* entity);
		/// <summary>
		/// Returns pointer to parent entity
		/// </summary>
		/// <returns></returns>
		Entity* GetParent() { return mParent; }
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

		const Vec3& GetPosition() { return mPosition; }
		const Vec3& GetScale() { return mScale; }
		const Vec3& GetRotation() { return mRotation; }

		void SetPosition(const Vec3& position);
		void SetScale(const Vec3& scale);
		void SetRotation(const Vec3& rotation);
		/// <summary>
		/// Destroy this object and all it's children
		/// </summary>
		void Destroy();
		/// <summary>
		/// Get amount of components, attached to this entity
		/// </summary>
		/// <returns></returns>
		uint32 GetComponentsCount() { return static_cast<uint32>(mComponents.size()); }
		/// <summary>
		/// Create and add new component to entity
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>pointer to new component</returns>
		template<typename T>
		T* AddComponent() {
			IEntityComponent* component = new T;
			component->SetEntity(this);
			
			mComponents.push_back(component);
			return static_cast<T*>(component);
		}

		void AddComponent(IEntityComponent* component) {
			component->SetEntity(this);
			mComponents.push_back(component);
		}
		/// <summary>
		/// Get entity component with type
		/// </summary>
		/// <typeparam name="T">type of component</typeparam>
		/// <returns></returns>
		template<typename T>
		T* GetComponent() {
			for (auto component : mComponents) {
				if (typeid(*component) == typeid(T)) {
					return static_cast<T*>(component);
				}
			}
			return nullptr;
		}

		template<typename T>
		bool HasComponent() {
			return GetComponent<T>() == nullptr ? false : true;
		}

		bool HasComponent(IEntityComponent* component);

		void RemoveComponent(IEntityComponent* component);

		template<class T>
		void RemoveComponent() {
			T* component = GetComponent<T>();
			if (component)
				RemoveComponent(component);
		}

		void RemoveAllComponents();
	};
}