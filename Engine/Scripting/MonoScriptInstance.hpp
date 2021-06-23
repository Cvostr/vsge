#pragma once

#include <mono/metadata/appdomain.h>
#include "MonoScript.hpp"

namespace VSGE {
	class MonoScriptInstance {
	private:
		MonoObject* _object;
		MonoScript* _script;
	public:
		MonoScriptInstance();

		MonoObject* GetObject();

		void CallConstructor();

		void CallStart();

		void CallUpdate();
		
		void CreateFromScript(MonoScript* script);
	};
}