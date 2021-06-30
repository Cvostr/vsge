#include "MonoScriptInstance.hpp"
#include "MonoLayer.hpp"
#include <Core/Time.hpp>

using namespace VSGE;

MonoScriptInstance::MonoScriptInstance() :
	_object(nullptr),
	_script(nullptr)
{

}

MonoObject* MonoScriptInstance::GetObject() {
	return _object;
}

void MonoScriptInstance::CreateFromScript(MonoScript* script) {
	MonoLayer* mono = MonoLayer::Get();
	_object = mono_object_new(mono->GetDomain(), script->GetMainClass());
	_script = script;
}

void MonoScriptInstance::CallConstructor() {
	if(_object)
		mono_runtime_object_init(_object);
}

void MonoScriptInstance::CallStart() {
	if(_script && _object)
		mono_runtime_invoke(_script->GetStartMethod(), _object, nullptr, nullptr);
}

void MonoScriptInstance::CallUpdate() {
	if (_script && _object) {
		float deltaTime = TimePerf::Get()->GetDeltaTime();
		void* args[1];
		args[0] = &deltaTime;

		mono_runtime_invoke(_script->GetUpdateMethod(), _object, args, nullptr);
	}
}