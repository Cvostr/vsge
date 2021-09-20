#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/VarTypes/Variant.hpp>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/debug-helpers.h>
#include <string>

namespace VSGE {
	class MonoScriptInstance {
	private:
		std::string _class_name;
		MonoClass* _mono_class_desc;
		MonoObject* _mono_class_instance;
		//base class methods
		MonoMethod* _start_method;
		MonoMethod* _update_method;

		MonoMethodDesc* GetMethodDesc(const std::string& method);
		MonoMethod* GetMethod(const std::string& method);
		MonoObject* CallMethod(MonoMethod* method_desc, void** params);
	public:

		MonoScriptInstance();
		~MonoScriptInstance();

		bool CreateClassByName(const std::string& class_name);
		void CallDefaultConstructor();
		const std::string& GetClassName();
		MonoClass* GetClassDesc();
		MonoMethod* GetStartMethod();
		MonoMethod* GetUpdateMethod();
		void Release();
	};
}