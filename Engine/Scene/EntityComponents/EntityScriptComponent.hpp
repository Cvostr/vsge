#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/ScriptResource.hpp>
#include <Scripting/Angel/AngelScriptInstance.hpp>
#include <vector>

namespace VSGE {

	class EntityScriptComponent : public IEntityComponent {
	private:
		std::string _scriptClassName;
		AngelScriptInstance* instance;
	public:
		EntityScriptComponent(){
			instance = new AngelScriptInstance;
		}

		~EntityScriptComponent() {
			delete instance;
		}

		void SetScriptName(const std::string& script);
		const std::string& GetScriptName();

		void OnUpdate();
		void OnStart();
		void OnDestroy();
		void OnPreRender();

		void OnActivate();
		void OnDeactivate();

		void Serialize(YAML::Emitter& e){}
		void Deserialize(YAML::Node& entity) {}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<EntityScriptComponent*> tEntityScriptsList;
}