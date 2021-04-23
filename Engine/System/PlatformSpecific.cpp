#include "PlatformSpecific.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace VSGE;

void VSGE::WIN32_DisableHIDPIWindowScale() {
#ifdef _WIN32
	SetProcessDPIAware();
#endif
}