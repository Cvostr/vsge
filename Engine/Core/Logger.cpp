#include "Logger.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#undef CreateWindow
#endif

#include <chrono>
#include <Engine/Application.hpp>

using namespace VSGE;
using namespace std;

#define INFO_PREFIX std::string("{INFO} ")
#define WARN_PREFIX string("{WARNING} ")
#define ERROR_PREFIX string("{ERROR} ")

OpLogger Logger::mOpLogger = OpLogger();

void SetConsoleColor(int color) {
#ifdef _WIN32
	HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(cout_handle, color | BACKGROUND_INTENSITY);
#endif
}

void PrintText(std::string text) {
	printf(text.c_str());
}

void Logger::Log(const std::string& msg, LogType type) {

	switch (type) {
	case LogType::LOG_TYPE_INFO:
		PrintText(INFO_PREFIX);
		break;
	case LogType::LOG_TYPE_WARN:
		PrintText(WARN_PREFIX);
		break;
	case LogType::LOG_TYPE_ERROR:
		PrintText(ERROR_PREFIX);
		break;
	}

	PrintText(msg + "\n");
}

OpLogger& Logger::Log(LogType type) {
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	MessageEvent* _event = new MessageEvent(type, now);

	mOpLogger.SetEvent(_event);
	Application::Get()->OnEvent(*_event);

	switch (type) {
	case LogType::LOG_TYPE_INFO:
		PrintText(INFO_PREFIX);
		break;
	case LogType::LOG_TYPE_WARN:
		PrintText(WARN_PREFIX);
		break;
	case LogType::LOG_TYPE_ERROR:
		PrintText(ERROR_PREFIX);
		break;
	}
	return mOpLogger;
}

OpLogger& Logger::GetOpLogger() {
	return mOpLogger;
}

OpLogger::OpLogger() {
	_lastEvent = nullptr;
	_print_to_stdout = true;
}
void OpLogger::SetEvent(MessageEvent* _event) {
	_lastEvent = _event;
}
void OpLogger::SetPrintToStdout(bool print) {
	_print_to_stdout = print;
}
OpLogger& OpLogger::operator<<(int var) {
	if(_print_to_stdout)
		printf("%i", var);
	_lastEvent->GetContent() += std::to_string(var);
	return *this;
}
OpLogger& OpLogger::operator<<(unsigned int var) {
	if (_print_to_stdout)
		printf("%u", var);
	_lastEvent->GetContent() += std::to_string(var);
	return *this;
}
OpLogger& OpLogger::operator<<(float var) {
	if (_print_to_stdout)
		printf("%f", var);
	_lastEvent->GetContent() += std::to_string(var);
	return *this;
}
OpLogger& OpLogger::operator<<(const char* var) {
	if (_print_to_stdout)
		printf("%s", var);
	_lastEvent->GetContent() += std::string(var);
	return *this;
}
OpLogger& OpLogger::operator<<(std::string var) {
	if (_print_to_stdout)
		printf("%s", var.c_str());
	_lastEvent->GetContent() += var;
	return *this;
}