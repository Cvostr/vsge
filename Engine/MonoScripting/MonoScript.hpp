#pragma once

#include <Core/VarTypes/Base.hpp>
#include <mono/metadata/appdomain.h>
#include <string>

namespace VSGE {
	class MonoScriptInstance {
	private:
		MonoClass* _mono_class;
		//base class methods
		MonoMethod* _start_method;
		MonoMethod* _update_method;

		MonoMethod* GetMethod(const std::string& method);
	public:

		MonoScriptInstance();
		~MonoScriptInstance();

		MonoClass* GetMainClass();
		MonoMethod* GetStartMethod();
		MonoMethod* GetUpdateMethod();
	};
}