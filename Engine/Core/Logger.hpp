#pragma once

#include <string>
#include <stdio.h>
#include <Engine/Event.hpp>
#include "VarTypes/Base.hpp"

namespace VSGE {

	enum class LogType {
		LOG_TYPE_NONE,
		LOG_TYPE_INFO,
		LOG_TYPE_WARN,
		LOG_TYPE_ERROR,
		LOG_TYPE_DEBUG,
		LOG_TYPE_SCRIPT_COMPILE_ERROR
	};

	class OpLogger;

	class MessageEvent : public IEvent {
	private:
		LogType _type;
		time_t _msg_time;
		std::string _content;
	public:

		EVENT_CLASS_TYPE(EventType::EventMessage)

		MessageEvent(LogType type, time_t time);
		MessageEvent(LogType type, time_t time, const std::string& msg);

		MessageEvent(const MessageEvent& other);
		MessageEvent(MessageEvent&& other) noexcept;

		LogType GetMsgType();
		const std::string& GetContent() const;
		time_t GetTime() const;
	};

	class Logger {
	private:
		static void PrintPrefix(LogType type);
	public:
		static void Log(const std::string& msg, LogType type = LogType::LOG_TYPE_INFO);
		static OpLogger Log(LogType type = LogType::LOG_TYPE_INFO);
	};

	class OpLogger {
	private:
		bool _print_to_stdout;

		LogType _pendingType;
		time_t _pendingTime;
		std::string _pendingContent;

		void SendEvent();
	public:
		OpLogger();
		~OpLogger();

		void NewEvent(LogType pendingType, time_t pendingTime);
		void SetPrintToStdout(bool print);

		OpLogger& operator<<(int var);
		OpLogger& operator<<(char var);
		OpLogger& operator<<(unsigned int var);
		OpLogger& operator<<(float var);
		OpLogger& operator<<(const char* var);
		OpLogger& operator<<(std::string var);

	};
}