#include "EntityScriptComponent.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

EntityScriptComponent::EntityScriptComponent() {
	_script_instance = new MonoScriptInstance;
}

EntityScriptComponent::~EntityScriptComponent() {
	SAFE_RELEASE(_script_instance)
}

void EntityScriptComponent::SetClassName(const std::string& class_name) {
	if (_class_name != class_name) {
		_class_name = class_name;
		LoadFields();
	}
}

void EntityScriptComponent::LoadFields() {
	_fields.clear();

	MonoScriptBlob* blob = MonoScriptingLayer::Get()->GetScriptsBlob();
	MonoClassDesc* class_desc = blob->GetMonoClassDesc(_class_name);

	if (class_desc == nullptr)
		return;

	for (auto& field_desc : class_desc->GetFields()) {
		if (field_desc->GetVisibility() == FIELD_PRIVATE)
			continue;
		MonoScriptField field(field_desc);
		_fields.push_back(field);
	}
}

const std::string& EntityScriptComponent::GetClassName() {
	return _class_name;
}

MonoScriptInstance* EntityScriptComponent::GetInstance() {
	return _script_instance;
}

std::vector<MonoScriptField>& EntityScriptComponent::GetFields() {
	return _fields;
}

MonoScriptField* EntityScriptComponent::GetFieldByNameType(const std::string& field_name, ValueType type) {
	for (auto& field : _fields) {
		if (field.GetDesc()->GetName() == field_name && field.GetDesc()->GetValueType() == type)
			return &field;
	}
	return nullptr;
}

void EntityScriptComponent::Init() {
	_script_instance->CreateClassByName(_class_name);
	_script_instance->CallDefaultConstructor();
	_script_instance->SetValuePtrToField("entity_ptr", &_entity);

	for (auto& field : _fields) {
		if (field.GetDesc()->GetValueType() != VALUE_TYPE_STRING)
			_script_instance->SetValuePtrToField(field.GetDesc()->GetName(), field.GetValue().GetValuePtr());
		else {
			MonoString* mono_string = MonoScriptingLayer::Get()->CreateMonoString(field.GetStringValue());
			_script_instance->SetValuePtrToField(field.GetDesc()->GetName(), mono_string);
		}
	}
}

void EntityScriptComponent::OnStart() {
	_script_instance->CallOnStart();
}

void EntityScriptComponent::OnStop() {
	_script_instance->Release();
}

void EntityScriptComponent::OnUpdate() {
	_script_instance->CallOnUpdate();
}

void EntityScriptComponent::OnTriggerStay(Entity* entity) {
	if(IsActive())
		_script_instance->CallOnTriggerStay(entity);
}

void EntityScriptComponent::OnTriggerEnter(Entity* entity) {
	if (IsActive())
		_script_instance->CallOnTriggerEnter(entity);
}

void EntityScriptComponent::OnTriggerExit(Entity* entity) {
	if (IsActive())
		_script_instance->CallOnTriggerExit(entity);
}

void EntityScriptComponent::OnScriptChanged(int step) {
	if (step == 0) {
		for (auto& field : _fields) {
			MonoClassFieldDesc* temp_desc = new MonoClassFieldDesc(
				field.GetDesc()->GetName(), 
				field.GetDesc()->GetValueType());
			field.SetDesc(temp_desc);
		}
	}
	else {
		std::vector<MonoScriptField> temp_fields;
		for (auto& field : _fields) {
			temp_fields.push_back(field);
		}

		LoadFields();

		for (auto& field : temp_fields) {
			//find field with this name and type
			MonoScriptField* pfield = GetFieldByNameType(field.GetDesc()->GetName(), field.GetDesc()->GetValueType());
			if (pfield) {
				pfield->GetValue().SetData(
					field.GetDesc()->GetValueType(),
					*((MultitypeData*)field.GetValue().GetValuePtr())
				);
				pfield->GetStringValue() = field.GetStringValue();
			}
			delete field.GetDesc();
		}
	}
}

void EntityScriptComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(GetClassName());
	serializer.Serialize((uint32)_fields.size());
	for (auto& field : _fields) {
		serializer.Serialize(field.GetDesc()->GetName());
		serializer.Serialize(field.GetDesc()->GetValueType());
		if (field.GetDesc()->GetValueType() != VALUE_TYPE_STRING)
			serializer.WriteBytes(field.GetValue().GetValuePtr(), sizeof(MultitypeData));
		else
			serializer.Serialize(field.GetStringValue());
	}
}
void EntityScriptComponent::Deserialize(ByteSolver& solver) {
	SetClassName(solver.ReadNextString());
	uint32 fields_count = solver.GetValue<uint32>();
	for (uint32 i = 0; i < fields_count; i++) {
		std::string field_name = solver.ReadNextString();
		ValueType type = solver.GetValue<ValueType>();
		MonoScriptField* field = GetFieldByNameType(field_name, type);
		if (!field)
			continue;
		if (type != VALUE_TYPE_STRING) {
			MultitypeData data = solver.GetValue<MultitypeData>();
			field->GetValue().SetData(type, data);
		}
		else {
			field->GetStringValue() = solver.ReadNextString();
		}
	}
}