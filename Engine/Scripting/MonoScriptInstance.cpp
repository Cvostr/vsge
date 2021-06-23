#include "MonoScriptInstance.hpp"
#include "MonoLayer.hpp"

using namespace VSGE;

MonoScriptInstance::MonoScriptInstance() :
	_object(nullptr)
{

}

MonoObject* MonoScriptInstance::GetObject() {
	return _object;
}

void MonoScriptInstance::CreateFromScript(MonoScript* script) {
	MonoLayer* mono = MonoLayer::Get();
	_object = mono_object_new(mono->GetDomain(), script->GetMainClass());
	//mono_runtime_object_init(script.object);
}