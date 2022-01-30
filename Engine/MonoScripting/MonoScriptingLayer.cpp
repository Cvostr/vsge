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

MonoScriptBlob* MonoScriptingLayer::GetScriptsBlob() {
    return _scripts_blob;
}

MonoDomain* MonoScriptingLayer::GetRootDomain() {
	return _root_domain;
}

MonoString* MonoScriptingLayer::CreateMonoString(const std::string& str) {
    return mono_string_new(_domain, str.c_str());
}

MonoString* MonoScriptingLayer::CreateMonoString(const String& str) {
    return mono_string_new_utf32(_domain, str.c_str(), str.Length());
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
    BindScenesApi();
    BindResourcesApi();
    BindInputApi();
    BindEntityComponentsApi();

    BindUiRenderList();

    BindNetworking();

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
        mono_domain_set(_root_domain, 0);
        #ifndef __linux__
        mono_domain_unload(_domain);
        #endif
        _domain = nullptr;
    }
}
void MonoScriptingLayer::AttachThread(){
    mono_thread_attach(mono_get_root_domain());
}
void MonoScriptingLayer::OnEvent(const VSGE::IEvent& event) {
    //dispatch events
    DispatchEvent<VSGE::NetworkClientConnectedEvent>
        (event, EVENT_FUNC(OnClientConnectedToServer));
    DispatchEvent<VSGE::NetworkClientDisconnectedEvent>
        (event, EVENT_FUNC(OnClientDisonnectedFromServer));

    for (auto& sub : subs_events) {
        if (sub.event_type == event.GetEventType()) {
            mono_runtime_invoke(sub.method_descr->GetMethod(),
                sub.mono_object, nullptr, nullptr);
        }
    }
}

void MonoScriptingLayer::OnClientConnectedToServer(const VSGE::NetworkClientConnectedEvent& event) {
    _network_state.server_ptr = event.GetServer();
    _network_state._client_id = event.GetConnectionId();
}

void MonoScriptingLayer::OnClientDisonnectedFromServer(const VSGE::NetworkClientDisconnectedEvent& event) {
    _network_state.server_ptr = event.GetServer();
    _network_state._client_id = event.GetConnectionId();
}

void MonoScriptingLayer::SubscribeToEvent(MonoObject* obj, EventType event_type, const std::string& method_name) {
    MonoClass* mono_class = mono_object_get_class(obj);
    std::string class_name = mono_class_get_name(mono_class);
    
    MonoClassDesc* class_desc = _scripts_blob->GetMonoClassDesc(class_name);

    MonoEventSubscription sub;
    sub.event_type = event_type;
    sub.mono_object = obj;
    sub.method_descr = class_desc->GetMethodDescByName(method_name);
    subs_events.push_back(sub);
}

NetworkEventsState& MonoScriptingLayer::GetNetworkEventState() {
    return _network_state;
}