#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Core/Logger.hpp>

namespace VSGEditor {
	class ConsoleWindow : public EditorWindow {
	private:
		std::vector<VSGE::MessageEvent> _infoMessages;
		std::vector<VSGE::MessageEvent> _warnMessages;
		std::vector<VSGE::MessageEvent> _errorMessages;
		std::vector<VSGE::MessageEvent> _scriptErrors;

		VSGE::LogType _type;

		std::vector<VSGE::MessageEvent>* GetVectorByType(VSGE::LogType type);
	public:

		ConsoleWindow();

		void AddMessage(VSGE::MessageEvent* msg_event);
		void ClearMessages(VSGE::LogType type);

		void OnDrawWindow();
	};
}