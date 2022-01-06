#pragma once

#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <vector>
#include <string>
#include "MonoClassFieldDesc.hpp"
#include "MonoEventDesc.hpp"

namespace VSGE {

	class MonoClassDesc {
	private:
		MonoClass* _class;
		std::string _name;
		std::string _namespace;
		std::vector<MonoClassFieldDesc> _fields;
		std::vector<MonoEventDesc> _events;
	public:

		MonoClass* GetMonoClass() const;

		const std::string& GetName() const;

		const std::string& GetNamespace() const;

		std::vector<MonoClassFieldDesc>& GetFields();

		std::vector<MonoEventDesc>& GetEvents();

		void Create(MonoClass* mono_class);

		MonoObject* CreateObjectInstance();

		MonoClassDesc();
		MonoClassDesc(MonoClass* mono_class);
		~MonoClassDesc();
	};
}