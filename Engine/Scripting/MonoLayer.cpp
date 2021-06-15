#include "MonoLayer.hpp"
#include <string>

using namespace VSGE;

void MonoLayer::OnAttach() {
    std::string dir_scripts = "";
    std::string dir_mono_lib = dir_scripts + std::string("mono\\lib");
    std::string dir_mono_etc = dir_scripts + std::string("mono\\etc");

    // Point mono to the libs and configuration files
    mono_set_dirs(dir_mono_lib.c_str(), dir_mono_etc.c_str());

	//_domain = mono_jit_init_version("VSGE_MONO", "v4.0.30319");
}
void MonoLayer::OnUpdate() {

}
void MonoLayer::OnDetach() {

}