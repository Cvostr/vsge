#pragma once

#include <mono/metadata/appdomain.h>
#include "MonoScript.hpp"

namespace VSGE {
	class MonoScriptInstance {
	private:
		MonoObject* _object;
	public:
		MonoScriptInstance();

		MonoObject* GetObject();

		void CreateFromScript(MonoScript* script);
	};
}