#include "MonoLayer.hpp"
#include <Core/Logger.hpp>
#include <string>

using namespace VSGE;

void MonoLayer::SetMonoDir(std::string dir) {
    _mono_dir = dir;
}

void MonoLayer::OnAttach() {
    

}
void MonoLayer::OnUpdate() {

}
void MonoLayer::OnDetach() {

}

bool MonoLayer::CreateDomain() {
    std::string dir_mono_lib = _mono_dir + std::string("/lib");
    std::string dir_mono_etc = _mono_dir + std::string("/etc");

    // Point mono to the libs and configuration files
    mono_set_dirs(dir_mono_lib.c_str(), dir_mono_etc.c_str());
    //Create domain
    _domain = mono_jit_init_version("VSGE_MONO", "v4.0.30319");

    if (_domain) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error creating mono domain\n";
        return false;
    }
    bool is_domain_set = mono_domain_set(_domain, false);
    if (!is_domain_set) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error setting base mono domain\n";
        return false;
    }

    mono_thread_set_main(mono_thread_current());

    _compiler_path = dir_mono_lib + "/mono/4.5/csc.exe";

    return true;
}