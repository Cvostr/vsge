#pragma once

#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <Core/VarTypes/Variant.hpp>

namespace VSGE {

	enum MonoClassFieldVisibility {
		FIELD_PRIVATE,
		FIELD_PROTECTED,
		FIELD_PUBLIC,
		FIELD_INTERNAL
	};

	class MonoClassDesc;

	ValueType ConvertStrToType(const std::string& str_type);

	class MonoClassFieldDesc {
	private:
		MonoClassDesc* _class;
		MonoClassField* _field;
		std::string _name;
		MonoClass* _type_class;
		MonoClassFieldVisibility _visibility;
		Variant _base_value;
	public:
		/// <summary>
		/// Get name of field as string
		/// </summary>
		/// <returns>name string</returns>
		const std::string& GetName() const;
		/// <summary>
		/// Get field's type name as string
		/// </summary>
		/// <returns></returns>
		const std::string GetTypeName() const;
		/// <summary>
		/// Get visibility of field
		/// </summary>
		/// <returns></returns>
		MonoClassFieldVisibility GetVisibility() const;
		/// <summary>
		/// Get desc of class containing this field
		/// </summary>
		/// <returns></returns>
		MonoClassDesc* GetClass() const;
		/// <summary>
		/// Get value of this field in specified class object
		/// </summary>
		/// <param name="object">- object of class</param>
		/// <returns></returns>
		MonoObject* GetValueMonoObject(MonoObject* object);

		void Create(MonoClassDesc* class_desc, MonoClassField* mono_field);

		ValueType GetValueType();
		Variant GetValue(MonoObject* object);

		MonoClassFieldDesc(const std::string& name, ValueType type);
		MonoClassFieldDesc(MonoClassDesc* class_desc, MonoClassField* mono_field);
		~MonoClassFieldDesc(){}
	};
}