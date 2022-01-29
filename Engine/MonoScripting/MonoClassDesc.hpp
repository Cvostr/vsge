#pragma once

#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <vector>
#include <string>
#include "MonoClassFieldDesc.hpp"
#include "MonoEventDesc.hpp"
#include "MonoMethodDesc.hpp"

namespace VSGE {

	class MonoClassDesc {
	private:
		MonoClass* _class;
		std::string _name;
		std::string _namespace;
		std::vector<MonoClassFieldDesc> _fields;
		std::vector<MonoEventDesc> _events;
		std::vector<MonoMethodDescr> _methods;
	public:

		MonoClass* GetMonoClass() const;

		const std::string& GetName() const;

		const std::string& GetNamespace() const;

		std::vector<MonoClassFieldDesc>& GetFields();

		std::vector<MonoEventDesc>& GetEvents();

		std::vector<MonoMethodDescr>& GetMethods();

		MonoMethodDescr* GetMethodDescByName(const std::string& method_name);

		void Create(MonoClass* mono_class);

		MonoObject* CreateObjectInstance();

		MonoClassDesc();
		MonoClassDesc(MonoClass* mono_class);
		~MonoClassDesc();
	};
}