#pragma once

#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

namespace VSGE {
	class MonoClassDesc;

	class MonoMethodDescr {
	private:
		std::string _name;
		MonoMethod* _mono_method;
		MonoClassDesc* _class;
	public:
		MonoMethodDescr(MonoClassDesc* class_desc, MonoMethod* mono_method);

		~MonoMethodDescr(){}

		const std::string& GetName() const;

		MonoMethod* GetMethod() const;
	};
}