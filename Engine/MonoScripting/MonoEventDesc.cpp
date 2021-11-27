#include "MonoEventDesc.hpp"

using namespace VSGE;

MonoEventDesc::MonoEventDesc(MonoClassDesc* class_desc, MonoEvent* mono_event) {
	Create(class_desc, mono_event);
}

const std::string& MonoEventDesc::GetName() const {
	return _name;
}
MonoClassDesc* MonoEventDesc::GetClass() const {
	return _class;
}
void MonoEventDesc::Create(MonoClassDesc* class_desc, MonoEvent* mono_event) {
	_name = std::string(mono_event_get_name(mono_event));
	_raise_method = mono_event_get_raise_method(mono_event);
	_event = mono_event;
}