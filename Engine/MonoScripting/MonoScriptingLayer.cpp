#include "MonoScriptingLayer.hpp"
#include <Core/Logger.hpp>
#include "Api/ApiBindings.hpp"

using namespace VSGE;

MonoScriptingLayer* MonoScriptingLayer::_this = nullptr;

MonoScriptingLayer::MonoScriptingLayer() :
    _root_domain(nullptr),
	_domain(nullptr),
    _scripts_blob(new MonoScriptBlob)
{
	_this = this;
    CreateRootDomain();
}
MonoScriptingLayer::~MonoScriptingLayer() {
    
}

void MonoScriptingLayer::OnAttach() {
    
}
void MonoScriptingLayer::OnUpdate() {

}
void MonoScriptingLayer::OnDetach() {

}

MonoDomain* MonoScriptingLayer::GetRootDomain() {
	return _root_domain;
}
bool MonoScriptingLayer::CreateRootDomain() {
    if (_root_domain)
        return true;
    mono_set_dirs("../MonoScripting/MonoBinaries/lib", "../MonoScripting/MonoBinaries/etc");
	//Create domain
    _root_domain = mono_jit_init_version(MONO_ROOT_DOMAIN_NAME, "v4.0.30319");
    if (!_root_domain) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error creating mono domain\n";
        return false;
    }
    bool is_domain_set = mono_domain_set(_root_domain, false);
    if (!is_domain_set) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error setting base mono domain\n";
        return false;
    }
    mono_thread_set_main(mono_thread_current());
    Logger::Log(LogType::LOG_TYPE_INFO) << "Mono Domain initialized\n";

    CreateDomain();

    //bind API
    BindLoggerApi();
    BindApplicationApi();
    BindMaterialObjectApi();
    BindEntityApi();
    BindSceneApi();
    BindResourcesApi();
    BindInputApi();
    BindEntityComponentsApi();

    BindUiRenderList();

    return true;
}

void MonoScriptingLayer::ReleaseRootDomain() {
    if (_root_domain) {
        mono_jit_cleanup(_root_domain);
        _root_domain = nullptr;
    }
}

MonoDomain* MonoScriptingLayer::GetDomain() {
    return _domain;
}
void MonoScriptingLayer::CreateDomain() {
    if (_domain)
        return;
    _domain = mono_domain_create_appdomain(MONO_DOMAIN_NAME, NULL);
    mono_domain_set(_domain, false);
}
void MonoScriptingLayer::ReleaseDomain() {
    if (_domain) {
        mono_domain_set(mono_get_root_domain(), 0);
        mono_domain_unload(_domain);
        _domain = nullptr;
    }
}