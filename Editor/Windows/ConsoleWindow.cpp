#include "ConsoleWindow.hpp"
#include <ctime>

using namespace VSGEditor;

void ConsoleWindow::OnDrawWindow() {
	if (Draw("Console")) {

		std::vector<VSGE::MessageEvent*>* msg_array = &_infoMessages;
		
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
}