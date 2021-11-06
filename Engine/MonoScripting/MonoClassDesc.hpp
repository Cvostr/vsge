#pragma once

#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <vector>
#include <string>
#include <Core/VarTypes/Variant.hpp>

namespace VSGE {

	enum MonoClassFieldVisibility {
		FIELD_PRIVATE,
		FIELD_PROTECTED,
		FIELD_PUBLIC,
		FIELD_INTERNAL
	};

	class MonoClassDesc;

	class MonoClassFieldDesc {
	private:
		MonoClassDesc* _class;
		MonoClassField* _field;
		std::string _name;
		MonoClass* _type_class;
		MonoClassFieldVisibility _visibility;
		ValueType _type;
	public:

		const std::string& GetName() const;

		const std::string GetTypeName() const;

		MonoClassFieldVisibility GetVisibility() const;

		MonoObject* GetValue(MonoObject* object);

		MonoClassFieldDesc(MonoClassDesc* class_desc, MonoClassField* mono_field);
	};

	class MonoClassDesc {
	private:
		MonoClass* _class;
		std::string _name;
		std::string _namespace;
		std::vector<MonoClassFieldDesc> _fields;
	public:

		MonoClass* GetMonoClass() const;

		const std::string& GetName() const;

		std::vector<MonoClassFieldDesc>& GetFields();

		void Create(MonoClass* mono_class);

		MonoObject* CreateObjectInstance();

		MonoClassDesc();
		MonoClassDesc(MonoClass* mono_class);
		~MonoClassDesc();
	};
}