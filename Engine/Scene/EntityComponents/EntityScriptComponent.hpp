#pragma once

#include "../IEntityComponent.hpp"
#include <vector>
#include <MonoScripting/MonoScript.hpp>

namespace VSGE {

	class EntityScriptComponent : public IEntityComponent {
	private:
		MonoScriptInstance* _script_instance;
	public:
		EntityScriptComponent();

		~EntityScriptComponent();
		
		void SetClassName(const std::string& class_name);

		const std::string& GetClassName();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<EntityScriptComponent*> tEntityScriptsList;
}