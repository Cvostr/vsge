#pragma once

#include "../Resource.hpp"

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

		void OnRelease() {
		}

		void PostLoad() {
		
		}

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_SCRIPT)

	};
}