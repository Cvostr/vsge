#define _CRT_SECURE_NO_WARNINGS

#include "ConsoleWindow.hpp"
#include <ctime>

using namespace VSGEditor;

ConsoleWindow::ConsoleWindow() {
	_type = VSGE::LogType::LOG_TYPE_INFO;
}

void ConsoleWindow::OnDrawWindow() {
	if (Draw("Console")) {

		std::vector<VSGE::MessageEvent>* msg_array = GetVectorByType(_type);

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
		
		for (auto& msg : *msg_array) {
			ImGui::Separator();
			time_t time = msg.GetTime();
			char* tm = std::ctime(&time);
			ImGui::Text("%s %s", tm, msg.GetContent().c_str());
		}

		// Auto scroll to down
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::End();
	}
}

std::vector<VSGE::MessageEvent>* ConsoleWindow::GetVectorByType(VSGE::LogType type)
{
	switch (type)
	{
	case VSGE::LogType::LOG_TYPE_INFO:
		return &_infoMessages;
	case VSGE::LogType::LOG_TYPE_ERROR:
		return &_errorMessages;
	case VSGE::LogType::LOG_TYPE_WARN:
		return &_warnMessages;
	case VSGE::LogType::LOG_TYPE_SCRIPT_COMPILE_ERROR:
		return &_scriptErrors;
	default:
		return nullptr;
	}
}

void ConsoleWindow::AddMessage(VSGE::MessageEvent* msg_event) 
{
	auto vec = GetVectorByType(msg_event->GetMsgType());
	if (vec != nullptr)
		vec->push_back(*msg_event);
}

void ConsoleWindow::ClearMessages(VSGE::LogType type) {
	auto vec = GetVectorByType(type);
	if (vec != nullptr)
		vec->clear();
}