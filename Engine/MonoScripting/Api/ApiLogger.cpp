#include "ApiBindings.hpp"
#include <Core/Logger.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void Log(MonoString* logString) 
{
	char* pLogString = mono_string_to_utf8(logString);
	std::string message = std::string(pLogString);
	mono_free(pLogString);

	Logger::Log(LogType::LOG_TYPE_INFO) << message << "\n";
}

void VSGE::BindLoggerApi() {
	MonoScriptingLayer::AddInternalCall("Logger::Log(string)", Log);
}