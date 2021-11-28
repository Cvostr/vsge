#pragma once

#include "../IEntityComponent.hpp"
#include <vector>
#include <MonoScripting/MonoScript.hpp>

namespace VSGE {

	class MonoScriptField {
	private:
		MonoClassFieldDesc* _desc;
		Variant _value;
		std::string _str_value;
	public:

		MonoClassFieldDesc* GetDesc() {
			return _desc;
		}

		void SetDesc(MonoClassFieldDesc* desc) {
			_desc = desc;
		}

		Variant& GetValue() {
			return _value;
		}

		std::string& GetStringValue() {
			return _str_value;
		}

		MonoScriptField(MonoClassFieldDesc* desc) {
			_desc = desc;
			_value.SetType(desc->GetValueType());
		}
	};

	class EntityScriptComponent : public IEntityComponent {
	private:
		MonoScriptInstance* _script_instance;
		std::string _class_name;

		std::vector<MonoScriptField> _fields;

		MonoScriptField* GetFieldByNameType(const std::string& field_name, ValueType type);
		void LoadFields();
	public:
		EntityScriptComponent();

		~EntityScriptComponent();
		/// <summary>
		/// Set class name to this entity script component
		/// All mono script fields configurations will be recreated
		/// </summary>
		/// <param name="class_name">- name of mono class</param>
		void SetClassName(const std::string& class_name);
		/// <summary>
		/// Get current picked mono script class name
		/// </summary>
		/// <returns></returns>
		const std::string& GetClassName();
		/// <summary>
		/// Get instance of mono script
		/// </summary>
		/// <returns></returns>
		MonoScriptInstance* GetInstance();
		/// <summary>
		/// Get list of class field instances
		/// </summary>
		/// <returns></returns>
		std::vector<MonoScriptField>& GetFields();
		/// <summary>
		/// Creates mono object and call constructor
		/// </summary>
		void Init();
		/// <summary>
		/// Calls OnCreate() on script class
		/// </summary>
		void OnStart();
		/// <summary>
		/// Calls OnStop() on script class
		/// </summary>
		void OnStop();
		/// <summary>
		/// Calls OnUpdate() on script class
		/// </summary>
		void OnUpdate();

		void OnScriptChanged(int step);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		void OnTriggerStay(Entity* entity);

		void OnTriggerEnter(Entity* entity);

		void OnTriggerExit(Entity* entity);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<EntityScriptComponent*> tEntityScriptsList;
}