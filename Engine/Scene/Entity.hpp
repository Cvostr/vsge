#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/VarTypes/Guid.hpp>
#include "IEntityComponent.hpp"
#include "EntityComponents/EntityScriptComponent.hpp"
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
		//Array of scripts, attached to entity
		tEntityScriptsList _scripts;
		//Array of children, attached to entity
		tEntityList _children; 

		AABB _boundingBox;
		Vec3 _position;
		Vec3 _scale;
		Quat _rotation;
		Mat4 _localTransform;
		Mat4 _worldTransform;
	public:

		Entity() :	_parent(nullptr),
					_scene(nullptr),
					_active(true),
					_static(false),
					_scale(1.f),
					_localTransform(1.f),
					_worldTransform(1.f),
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
		void SetGuid(const Guid& id);
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
		/// <summary>
		/// Get view mask of this entity
		/// </summary>
		/// <returns></returns>
		ViewMask GetViewMask() const;
		/// <summary>
		/// Set view mask to this entity
		/// </summary>
		/// <param name="viewMask"></param>
		void SetViewMask(ViewMask viewMask);
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
		uint32 GetChildrenCount() const;
		/// <summary>
		/// Get amount of children in all relations
		/// </summary>
		/// <returns></returns>
		uint32 GetTotalChildrenCount() const;
		/// <summary>
		/// Return child entity by index.
		/// </summary>
		/// <returns></returns>
		Entity** GetChildren() { return _children.data(); }
		/// <summary>
		/// Get entity's child at specified index
		/// </summary>
		/// <param name="child">- index of child</param>
		/// <returns></returns>
		Entity* GetChild(uint32 child) { return _children[child]; }
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
		Entity* GetEntityWithName(const std::string& name);
		/// <summary>
		/// Tries to find the entity with the given Guid in this entity hierarchy
		/// </summary>
		/// <param name="id">- guid to find</param>
		/// <returns>pointer to entity</returns>
		Entity* GetEntityWithGuid(const Guid& id);
		/// <summary>
		/// Create new entity with same content
		/// </summary>
		/// <returns>pointer to new entity</returns>
		Entity* Dublicate();
		/// <summary>
		/// Store entity in prefab
		/// </summary>
		/// <param name="out">-prefab bytes</param>
		/// <param name="size">-size of prefab</param>
		void ToPrefab(byte** out, uint32& size);
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
		/// <summary>
		/// Get absolute rotation in quaternion of entity
		/// </summary>
		/// <returns></returns>
		Quat GetAbsoluteRotation() const;
		/// <summary>
		/// Set new position for entity
		/// </summary>
		/// <param name="position">- new position</param>
		void SetPosition(const Vec3& position);
		/// <summary>
		/// Set new scale for entity
		/// </summary>
		/// <param name="scale">- new scale</param>
		void SetScale(const Vec3& scale);
		/// <summary>
		/// Set new rotation for entity
		/// </summary>
		/// <param name="rotation">- new rotation</param>
		void SetRotation(const Quat& rotation);
		/// <summary>
		/// Set new rotation for entity in euler angles
		/// </summary>
		/// <param name="rotation">- new rotation</param>
		void SetRotationEuler(const Vec3& rotation);
		/// <summary>
		/// Get Axis aligned bounding box of this entity
		/// </summary>
		/// <param name="extendChildren"></param>
		/// <returns></returns>
		const AABB& GetAABB(bool extendChildren = true);
		/// <summary>
		/// Get local transform matrix
		/// </summary>
		/// <returns></returns>
		const Mat4& GetLocalTransform() { return _localTransform; }
		/// <summary>
		/// Set local transform matrix
		/// </summary>
		/// <param name="transform">- new transform matrix</param>
		void SetLocalTransform(const Mat4& transform) { _localTransform = transform; }
		/// <summary>
		/// Get world transform matrix
		/// </summary>
		/// <returns></returns>
		const Mat4& GetWorldTransform() const;
		/// <summary>
		/// Set world transform matrix to entity, without updating pos, scale, rotation
		/// </summary>
		/// <param name="transform"></param>
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
		uint32 GetComponentsCount();
		/// <summary>
		/// Get amount of scripts, attached to this entity
		/// </summary>
		/// <returns></returns>
		uint32 GetScriptsCount();
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

		IEntityComponent** GetComponents();

		EntityScriptComponent** GetScripts();

		void AddScript(EntityScriptComponent* script);

		void RemoveScript(EntityScriptComponent* script);

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
		void CallOnTriggerStay(Entity* entity);
		void CallOnTriggerEnter(Entity* entity);
		void CallOnTriggerExit(Entity* entity);
		void CallOnScriptChanged(int step);

		//-----SKINNING----
		Entity* GetRootSkinningEntity();
		Entity* GetChildEntityWithLabel(const std::string& label);
	};
}
