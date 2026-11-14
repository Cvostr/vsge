#include "Logger.hpp"
#include <chrono>

using namespace VSGE;

MessageEvent::MessageEvent(LogType type, time_t time) :
	_type(type),
	_msg_time(time)
{
}

MessageEvent::MessageEvent(LogType type, time_t time, const std::string& msg) :
	_type(type),
	_msg_time(time),
	_content(msg)
{
}

MessageEvent::MessageEvent(LogType type, const std::string& msg) :
	_type(type),
	_content(msg)
{
	_msg_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

MessageEvent::MessageEvent(const MessageEvent& other) :
	_type(other._type),
	_msg_time(other._msg_time),
	_content(other._content)
{
}

MessageEvent::MessageEvent(MessageEvent&& other) noexcept :
	_type(other._type),
	_msg_time(other._msg_time),
	_content(std::move(other._content))
{
}

LogType MessageEvent::GetMsgType() {
	return _type;
}

const std::string& MessageEvent::GetContent() const {
	return _content;
}

time_t MessageEvent::GetTime() const {
	return _msg_time;
}