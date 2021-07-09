#pragma once

#include "../Resource.hpp"
#include <Scripting/Angel/AngelScriptLayer.hpp>

namespace VSGE {
	class ScriptResource : public Resource {
	private:
		bool _resource_compiled;
	public:

		ScriptResource() {
			_resource_compiled = true;
		}

		~ScriptResource() {
		}

		void SetCompiled(bool compiled) {
			_resource_compiled = compiled;
		}

		void Release() {
		}

		void PostLoad() {
			AngelScriptLayer* layer = AngelScriptLayer::Get();
			AngelScriptModule* module = layer->GetModule();
			module->AddScript(_loadedData, _description.size, this->GetName());
		}

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_SCRIPT)

	};
}