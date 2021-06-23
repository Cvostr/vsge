#pragma once

#include "../Resource.hpp"
#include <Scripting/MonoScript.hpp>

namespace VSGE {
	class ScriptResource : public Resource {
	private:
		MonoScript* _script;
	private:

		ScriptResource() {
			_script = new MonoScript;
		}

		~ScriptResource() {
			delete _script;
		}

		MonoScript* GetScript() {
			return _script;
		}

		void Release() {
			//_texture->Destroy();
		}

		void PostLoad() {
			
			//_texture->CreateFromBuffer(_loadedData, _description.size);
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_SCRIPT;
		}

	}
}