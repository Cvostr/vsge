#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/ScriptResource.hpp>
#include <vector>

namespace VSGE {

	class EntityScriptComponent : public IEntityComponent {
	private:
		
	public:
		EntityScriptComponent(){
		}

		~EntityScriptComponent() {
		}
		
		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<EntityScriptComponent*> tEntityScriptsList;
}