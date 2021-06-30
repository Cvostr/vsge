#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/VarTypes/Guid.hpp>
#include "IEntityComponent.hpp"
#include <string>
#include <typeinfo>
#include "Math/Vec3.hpp"
#include <Math/Quaternion.hpp>
#include "Math/Mat4.hpp"
#include "Math/AABB.hpp"
#include "Camera.hpp"

namespace VSGE {

	class Scene;
	class Entity;

	typedef std::vector<Entity*> tEntityList;

	class Entity  {
	private:

		bool _active;
		bool _static;
		ViewMask _viewMask;
		std::string _name;
		Guid _id;

		//Parent of Entity, NULL if root entity
		Entity* _parent; 
		//Scene, that contains this entity
		Scene* _scene; 
		//Array of components, attached to entity
		tComponentList _components;
		//Array of children, attached to entity
		tEntityList _children; 

		AABB _boundingBox;
		Vec3 _position;
		Vec3 _scale;
		Quat _rotation;
		Mat4 LocalTransform;
		Mat4 WorldTransform;
		bool mTransformDirty;
	public:

		Entity() :	_parent(nullptr),
					_scene(nullptr),
					_active(true),
					_static(false),
					mTransformDirty(false),
					_scale(1.f),
					LocalTransform(1.f),
					WorldTransform(1.f),
					_viewMask(MAX_INT64)
		{}

		~Entity() {

		}

		/// <summary>
		/// get guid of entity
		/// </summary>
		/// <returns>entity's guid</returns>
		const Guid& GetGuid() const;
		/// <summary>
		/// Set new guid to entity
		/// </summary>
		/// <param name="id"></param>
		void SetGuid(Guid& id);
		/// <summary>
		/// Stores pointer to scene in entity class
		/// </summary>
		/// <param name="scene"> - pointer to scene</param>
		void SetScene(Scene* scene);
		/// <summary>
		/// Get pointer to scene in entity class
		/// </summary>
		/// <returns>pointer to scene</returns>
		Scene* GetScene() const;
		/// <summary>
		/// Stores pointer to parent entity in entity class
		/// </summary>
		/// <param name="parent"> - pointer to parent entity</param>
		void SetParent(Entity* parent);
		/// <summary>
		/// Set active/inactive state of entity
		/// </summary>
		/// <param name="active"></param>
		void SetActive(bool active);
		/// <summary>
		/// Return whether entity is active.
		/// </summary>
		/// <returns></returns>
		bool IsActive() const;
		/// <summary>
		/// Set static/dynamic param of entity
		/// </summary>
		/// <param name="static"></param>
		void SetStatic(bool _static);
		/// <summary>
		/// Return whether entity is static.
		/// </summary>
		/// <returns></returns>
		bool IsStatic() const;
		/// <summary>
		/// Set new name string to entity
		/// </summary>
		/// <param name="name">- new name string</param>
		void SetName(const std::string& name);
		/// <summary>
		/// get name of entity
		/// </summary>
		/// <returns></returns>
		const std::string& GetName() const;

		ViewMask GetViewMask() {
			return _viewMask;
		}
		void SetViewMask(ViewMask viewMask) {
			_viewMask = viewMask;
		}
		/// <summary>
		/// Add new child to this entity
		/// </summary>
		/// <param name="entityToAdd">- pointer to new child</param>
		void AddChild(Entity* entityToAdd, bool retransform = true);
		/// <summary>
		/// Remove child from this entity
		/// </summary>
		/// <param name="entityToRemove">- pointer to child entity to remove</param>
		void RemoveChild(Entity* entityToRemove);
		/// <summary>
		/// Get amount of children, connected to this object
		/// </summary>
		/// <returns></returns>
		uint32 GetChildrenCount() const { return static_cast<uint32>(_children.size()); }
		/// <summary>
		/// Return child entity by index.
		/// </summary>
		/// <returns></returns>
		Entity** GetChildren() { return _children.data(); }
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
		Entity* GetParent() const;
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

		Entity* Dublicate();

		void ToPrefab(char** out);
		/// <summary>
		/// Get local position of entity
		/// </summary>
		/// <returns></returns>
		const Vec3& GetPosition() const { return _position; }
		/// <summary>
		/// Get local scale of entity
		/// </summary>
		/// <returns></returns>
		const Vec3& GetScale() const { return _scale; }
		/// <summary>
		/// Get local rotation of entity
		/// </summary>
		/// <returns></returns>
		const Quat& GetRotation() const { return _rotation; }
		/// <summary>
		/// Get local rotation of entity in euler angles
		/// </summary>
		/// <returns></returns>
		Vec3 GetRotationEuler() const { return _rotation.GetEulerAngles(); }
		/// <summary>
		/// Get absolute position of entity
		/// </summary>
		/// <returns></returns>
		Vec3 GetAbsolutePosition() const;
		/// <summary>
		/// Get absolute scale of entity
		/// </summary>
		/// <returns></returns>
		Vec3 GetAbsoluteScale() const;
		Quat GetAbsoluteRotation();

		void SetPosition(const Vec3& position);
		void SetScale(const Vec3& scale);
		void SetRotation(const Quat& rotation);
		void SetRotationEuler(const Vec3& rotation);

		const AABB& GetAABB(bool extendChildren = true);

		const Mat4& GetLocalTransform() { return LocalTransform; }
		void SetLocalTransform(const Mat4& transform) { LocalTransform = transform; }

		const Mat4& GetWorldTransform() { return WorldTransform; }
		void SetWorldTransform(const Mat4& transform);
		/// <summary>
		/// Recalculate local and world-space matrices
		/// </summary>
		void UpdateTransformMatrices();
		/// <summary>
		/// Destroy this object and all it's children
		/// </summary>
		void Destroy();
		/// <summary>
		/// Get amount of components, attached to this entity
		/// </summary>
		/// <returns></returns>
		uint32 GetComponentsCount() { return static_cast<uint32>(_components.size()); }
		/// <summary>
		/// Create and add new component to entity
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>pointer to new component</returns>
		template<typename T>
		T* AddComponent() {
			IEntityComponent* component = new T;
			component->SetEntity(this);
			
			_components.push_back(component);
			return static_cast<T*>(component);
		}

		void AddComponent(IEntityComponent* component);
		/// <summary>
		/// Get entity component with type
		/// </summary>
		/// <typeparam name="T">type of component</typeparam>
		/// <returns></returns>
		template<typename T>
		T* GetComponent() {
			for (auto component : _components) {
				if (typeid(*component) == typeid(T)) {
					return static_cast<T*>(component);
				}
			}
			return nullptr;
		}

		IEntityComponent** GetComponents() {
			return _components.data();
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

		void CallOnStart();
		void CallOnUpdate();
		void CallOnStop();
		void CallOnPreRender();

		//-----SKINNING----
		Entity* GetRootSkinningEntity();
		Entity* GetChildEntityWithLabel(const std::string& label);
	};
}