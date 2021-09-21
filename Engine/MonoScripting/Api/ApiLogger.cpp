#include "ApiBindings.hpp"
#include <Core/Logger.hpp>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/debug-helpers.h>

using namespace VSGE;

static void Log(MonoString* mono_string) {
	std::string message = std::string(mono_string_to_utf8(mono_string));
	Logger::Log(LogType::LOG_TYPE_INFO) << message << "\n";
}

void VSGE::BindLoggerApi() {
	mono_add_internal_call("Logger::Log(string)", Log);
}