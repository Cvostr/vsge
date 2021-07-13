#include "AngelScriptInstance.hpp"
#include "AngelScriptLayer.hpp"

using namespace VSGE;

AngelScriptInstance::AngelScriptInstance() {
	_mainclass_obj = nullptr;
	_class_desc = nullptr;
}

AngelScriptInstance::~AngelScriptInstance() {
	_mainclass_obj->Release();
}

void AngelScriptInstance::CreateInstance(const std::string& class_name, Entity* entity) {
	if (_mainclass_obj) {
		_mainclass_obj->Release();
	}

	AngelScriptLayer* layer = AngelScriptLayer::Get();
	layer->GetModule()->CreateMainClassObject(class_name, entity, &_mainclass_obj);
}

void AngelScriptInstance::CreateInstance(MainClassDesc* class_desc, Entity* entity) {
	if (_mainclass_obj) {
		_mainclass_obj->Release();
	}

	AngelScriptLayer* layer = AngelScriptLayer::Get();
	layer->GetModule()->CreateMainClassObject(class_desc, entity, &_mainclass_obj);
}

asIScriptObject* AngelScriptInstance::GetMainClassObject() {
	return _mainclass_obj;
}

void AngelScriptInstance::CallOnStart() {
	asIScriptFunction* func = _class_desc->GetFuncOnMainClass("void Start()");
	AngelScriptLayer* layer = AngelScriptLayer::Get();

	if (func) {
		layer->SetContextFunction(func);
		//layer->ContextSetObject();
		layer->ContextExecute();
	}
}
void AngelScriptInstance::CallOnStop() {
	asIScriptFunction* func = _class_desc->GetFuncOnMainClass("void Stop()");
}
void AngelScriptInstance::CallOnUpdate() {
	asIScriptFunction* func = _class_desc->GetFuncOnMainClass("void Update()");
	AngelScriptLayer* layer = AngelScriptLayer::Get();

	if (func) {
		layer->SetContextFunction(func);
		//layer->ContextSetObject();
		layer->ContextExecute();
	}
}

void AngelScriptInstance::CallOnTrigger(Entity* obj) {

}
void AngelScriptInstance::CallOnTriggerEnter(Entity* obj) {

}
void AngelScriptInstance::CallOnTriggerExit(Entity* obj) {

}