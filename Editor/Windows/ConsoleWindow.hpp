#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Core/Logger.hpp>

namespace VSGEditor {
	class ConsoleWindow : public EditorWindow {
	private:
		std::vector<VSGE::MessageEvent*> _infoMessages;
		std::vector<VSGE::MessageEvent*> _warnMessages;
		std::vector<VSGE::MessageEvent*> _errorMessages;

		VSGE::LogType _type;
	public:

		void addMsg(VSGE::MessageEvent* msg_event);

		void OnDrawWindow();
	};
}