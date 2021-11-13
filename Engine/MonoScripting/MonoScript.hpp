#pragma once

#include <Core/VarTypes/Base.hpp>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/debug-helpers.h>
#include <string>
#include "MonoClassDesc.hpp"

namespace VSGE {

	class MonoScriptInstance {
	private:
		std::string _class_name;
		MonoClassDesc* _mono_class_desc;
		MonoObject* _mono_class_instance;
		//base class methods
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
		MonoMethod* GetUpdateMethod();
		MonoObject* GetObject();

		void SetValuePtrToField(const std::string field, void* value);
		void SetStringToField(const std::string field, const String& str);
		void SetStringToField(const std::string field, const std::string& str);

		void* GetValueOfField(const std::string& field);

		template<typename T>
		void SetValueToField(const std::string& field, T value) {
			SetValuePtrToField(field, &value);
		}
		template<typename T>
		T GetValueOfField(const std::string& field) {
			T* value = SetValueToField(field, &value);
			return *value;
		}

		void CallOnStart();
		void CallOnUpdate();
		void CallOnTriggerStay(void* entity);
		void CallOnTriggerEnter(void* entity);
		void CallOnTriggerExit(void* entity);

		void Release();
	};
}