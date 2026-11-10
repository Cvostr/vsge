#include "ApiBindings.hpp"
#include <Core/Logger.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void Log(LogType type, MonoString* logString) 
{
	char* pLogString = mono_string_to_utf8(logString);
	std::string message = std::string(pLogString);
	mono_free(pLogString);

	Logger::Log(type) << message << "\n";
}

void VSGE::BindLoggerApi() {
	MonoScriptingLayer::AddInternalCall("Logger::Log(MessageType,string)", Log);
}