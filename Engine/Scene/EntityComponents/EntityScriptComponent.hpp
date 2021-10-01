#pragma once

#include "../IEntityComponent.hpp"
#include <vector>
#include <MonoScripting/MonoScript.hpp>

namespace VSGE {

	class EntityScriptComponent : public IEntityComponent {
	private:
		MonoScriptInstance* _script_instance;
		std::string _class_name;
	public:
		EntityScriptComponent();

		~EntityScriptComponent();
		
		void SetClassName(const std::string& class_name);

		const std::string& GetClassName();

		MonoScriptInstance* GetInstance();
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

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<EntityScriptComponent*> tEntityScriptsList;
}