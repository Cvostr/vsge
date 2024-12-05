#include "MonoScriptingLayer.hpp"
#include <Core/Logger.hpp>
#include "Api/ApiBindings.hpp"
#include <optional>
#include "mpi/Filesystem/File.hpp"

using namespace VSGE;

MonoScriptingLayer* MonoScriptingLayer::_this = nullptr;

#define RUNTIME_VERSION "v4.0.30319"

MonoScriptingLayer::MonoScriptingLayer() :
    _root_domain(nullptr),
	_domain(nullptr),
    _scripts_blob(new MonoScriptBlob)
{
	_this = this;
}
MonoScriptingLayer::~MonoScriptingLayer() 
{
    SAFE_RELEASE(_scripts_blob)
}

void MonoScriptingLayer::OnAttach() 
{
    CreateRootDomain();
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

void MonoScriptingLayer::SetupDebugging()
{
    const char* options[] = {
        "--soft-breakpoints",
        "--debugger-agent=transport=dt_socket,address=127.0.0.1:55555,embedding=1,server=y,suspend=n"
    };
    mono_debug_init(MONO_DEBUG_FORMAT_MONO);
    mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);
}

bool MonoScriptingLayer::CreateRootDomain() {
    if (_root_domain)
        return true;

    std::vector<std::string> paths = { 
        "../MonoScripting/MonoBinaries", 
        "../../../../MonoScripting/MonoBinaries",   // Visual Studio CMake
        "MonoScripting/MonoBinaries", 
        "MonoBinaries" };

    std::optional<std::string> monoRootDir = std::nullopt;
    for (const auto& path : paths) {
        Mpi::File dir(path);
        if (dir.isDirectory()) {
            monoRootDir = path;
        }
    }

    if (!monoRootDir.has_value()) {
        throw std::exception("Can't find Mono directory\n");
    }

    _monoDirectoryLocation = monoRootDir.value();
    std::string monoLibDir = _monoDirectoryLocation + "/lib";
    std::string monoEtcDir = _monoDirectoryLocation + "/etc";

    mono_set_dirs(monoLibDir.c_str(), monoEtcDir.c_str());

    // Init debugging, should be done before domain creation
    SetupDebugging();

	// Create domain
    _root_domain = mono_jit_init_version(MONO_ROOT_DOMAIN_NAME, RUNTIME_VERSION);
    if (!_root_domain) {
        throw std::exception("Error creating mono domain");
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

    return true;
}

void MonoScriptingLayer::ReleaseRootDomain() {
    if (_root_domain) {
        mono_jit_cleanup(_root_domain);
        _root_domain = nullptr;
    }
}

const std::string& MonoScriptingLayer::GetMonoDirectoryPath() const
{
    return _monoDirectoryLocation;
}

MonoDomain* MonoScriptingLayer::GetDomain() 
{
    return _domain;
}

void MonoScriptingLayer::CreateDomain() 
{
    if (_domain)
        return;

    _domain = mono_domain_create_appdomain(MONO_DOMAIN_NAME, NULL);
    mono_domain_set(_domain, false);
}

void MonoScriptingLayer::ReleaseDomain() 
{
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