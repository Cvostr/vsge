#include "ApiBindings.hpp"
#include <Core/Logger.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void Log(MonoString* mono_string) {
	std::string message = std::string(mono_string_to_utf8(mono_string));
	Logger::Log(LogType::LOG_TYPE_INFO) << message << "\n";
}

void VSGE::BindLoggerApi() {
	MonoScriptingLayer::AddInternalCall("Logger::Log(string)", Log);
}