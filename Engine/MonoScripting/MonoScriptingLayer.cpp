#include "MonoScriptingLayer.hpp"
#include <Core/Logger.hpp>

using namespace VSGE;

MonoScriptingLayer* MonoScriptingLayer::_this = nullptr;

MonoScriptingLayer::MonoScriptingLayer() :
	_domain(nullptr),
    _scripts_blob(new MonoScriptBlob)
{
	_this = this;
}
MonoScriptingLayer::~MonoScriptingLayer() {
    
}

void MonoScriptingLayer::OnAttach() {

}
void MonoScriptingLayer::OnUpdate() {

}
void MonoScriptingLayer::OnDetach() {

}

MonoDomain* MonoScriptingLayer::GetDomain() {
	return _domain;
}
bool MonoScriptingLayer::CreateDomain() {
	mono_set_dirs("../MonoScripting/MonoBinaries/lib", "../MonoScripting/MonoBinaries/etc");
	//Create domain
	_domain = mono_jit_init_version("VSGE_MONO", "v4.0.30319");
    if (!_domain) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error creating mono domain\n";
        return false;
    }
    bool is_domain_set = mono_domain_set(_domain, false);
    if (!is_domain_set) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error setting base mono domain\n";
        return false;
    }
    mono_thread_set_main(mono_thread_current());
    Logger::Log(LogType::LOG_TYPE_INFO) << "Mono Domain initialized\n";

    return true;
}

void MonoScriptingLayer::ReleaseDomain() {
    if (_domain) {
        mono_jit_cleanup(_domain);
        _domain = nullptr;
    }
}