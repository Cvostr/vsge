#define _CRT_SECURE_NO_WARNINGS

#include "ConsoleWindow.hpp"
#include <ctime>

using namespace VSGEditor;

void ConsoleWindow::OnDrawWindow() {
	if (Draw("Console")) {


		std::vector<VSGE::MessageEvent*>* msg_array = &_infoMessages;
		if (_type == VSGE::LogType::LOG_TYPE_ERROR) {
			msg_array = &_errorMessages;
		}
		if (_type == VSGE::LogType::LOG_TYPE_WARN) {
			msg_array = &_warnMessages;
		}
		if (_type == VSGE::LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) {
			msg_array = &_scriptErrors;
		}

		if (ImGui::Button("Clear")) {
			ClearMessages(_type);
		}
		ImGui::SameLine();
		if (ImGui::Button("INFO")) {
			_type = VSGE::LogType::LOG_TYPE_INFO;
		}
		ImGui::SameLine();
		if (ImGui::Button("WARN")) {
			_type = VSGE::LogType::LOG_TYPE_WARN;
		}
		ImGui::SameLine();
		if (ImGui::Button("ERROR")) {
			_type = VSGE::LogType::LOG_TYPE_ERROR;
		}
		ImGui::SameLine();
		if (ImGui::Button("Scripts Errors")) {
			_type = VSGE::LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR;
		}
		
		for (auto msg : *msg_array) {
			ImGui::Separator();
			time_t time = msg->GetTime();
			char* tm = std::ctime(&time);
			ImGui::Text("%s %s", tm, msg->GetContent().c_str());
		}

		ImGui::End();
	}
}


void ConsoleWindow::addMsg(VSGE::MessageEvent* msg_event) {
	if (msg_event->GetMsgType() == VSGE::LogType::LOG_TYPE_INFO) {
		_infoMessages.push_back(msg_event);
	}
	if (msg_event->GetMsgType() == VSGE::LogType::LOG_TYPE_ERROR) {
		_errorMessages.push_back(msg_event);
	}
	if (msg_event->GetMsgType() == VSGE::LogType::LOG_TYPE_WARN) {
		_warnMessages.push_back(msg_event);
	}
	if (msg_event->GetMsgType() == VSGE::LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) {
		_scriptErrors.push_back(msg_event);
	}
}

void ConsoleWindow::ClearMessages(VSGE::LogType type) {
	std::vector<VSGE::MessageEvent*>* msg_array = &_infoMessages;
	if (_type == VSGE::LogType::LOG_TYPE_ERROR) {
		msg_array = &_errorMessages;
	}
	if (_type == VSGE::LogType::LOG_TYPE_WARN) {
		msg_array = &_warnMessages;
	}
	if (_type == VSGE::LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR) {
		msg_array = &_scriptErrors;
	}
	msg_array->clear();
}