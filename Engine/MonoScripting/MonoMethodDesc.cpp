#include "MonoMethodDesc.hpp"

using namespace VSGE;

MonoMethodDescr::MonoMethodDescr(MonoClassDesc* class_desc, MonoMethod* mono_method) {
	_class = class_desc;
	_mono_method = mono_method;
	_name = std::string(mono_method_get_name(mono_method));
}

const std::string& MonoMethodDescr::GetName() const {
	return _name;
}

MonoMethod* MonoMethodDescr::GetMethod() const {
	return _mono_method;
}