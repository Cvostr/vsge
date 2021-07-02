#pragma once

#include "../IEntityComponent.hpp"
#include <vector>

namespace VSGE {

	class IEntityScript : public IEntityComponent {
	private:

	public:
		IEntityScript(){}

		virtual void OnUpdate() = 0;
		virtual void OnStart() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnPreRender() = 0;

		virtual void OnActivate() = 0;
		virtual void OnDeactivate() = 0;

		virtual void Serialize(YAML::Emitter& e) = 0;
		virtual void Deserialize(YAML::Node& entity) = 0;

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_SCRIPT, "Script")
	};

	typedef std::vector<IEntityScript*> tEntityScriptsList;
}