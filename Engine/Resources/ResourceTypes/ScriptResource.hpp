#pragma once

#include "../Resource.hpp"
#include <Scripting/MonoScript.hpp>

namespace VSGE {
	class ScriptResource : public Resource {
	private:
		MonoScript* _script;
		bool _resource_compiled;
	public:

		ScriptResource() {
			_script = new MonoScript;
			_resource_compiled = true;
		}

		~ScriptResource() {
			delete _script;
		}

		void SetCompiled(bool compiled) {
			_resource_compiled = compiled;
		}

		MonoScript* GetScript() {
			return _script;
		}

		void Release() {
			//_texture->Destroy();
		}

		void PostLoad() {
			_script->CreateFromSourceFile(_description.file_path);
			//_texture->CreateFromBuffer(_loadedData, _description.size);
		}

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_SCRIPT)

	};
}