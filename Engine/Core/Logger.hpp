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

		MessageEvent(LogType type, time_t time) :
			_type(type),
			_msg_time(time)
		{}

		LogType GetMsgType() {
			return _type;
		}
		std::string& GetContent() {
			return _content;
		}
		time_t GetTime() {
			return _msg_time;
		}
	};

	class Logger {
	private:
		static OpLogger mOpLogger;
	public:
		static void Log(const std::string& msg, LogType type = LogType::LOG_TYPE_INFO);
		static OpLogger& Log(LogType type = LogType::LOG_TYPE_INFO);
	};

	class OpLogger {
	private:
		MessageEvent* _lastEvent;
	public:
		OpLogger();

		void SetEvent(MessageEvent* _event);

		OpLogger& operator<<(int var);
		OpLogger& operator<<(unsigned int var);
		OpLogger& operator<<(float var);
		OpLogger& operator<<(const char* var);
		OpLogger& operator<<(std::string var);

	};
}