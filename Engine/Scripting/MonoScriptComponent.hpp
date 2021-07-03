#pragma once

#include <Scene/EntityComponents/EntityScriptComponent.hpp>
#include "MonoScriptInstance.hpp"

namespace VSGE {
	class MonoScriptComponent : public IEntityScript {
	private:
		MonoScriptInstance* _instance;
	public:

		MonoScriptComponent();
		
		void OnUpdate();
		void OnStart();
		void OnDestroy();
		void OnPreRender();

		void OnActivate();
		void OnDeactivate();

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);
	};
}