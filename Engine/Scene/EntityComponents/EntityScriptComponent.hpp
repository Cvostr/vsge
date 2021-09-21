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

		void OnStart();

		void OnStop();

		void OnUpdate();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<EntityScriptComponent*> tEntityScriptsList;
}