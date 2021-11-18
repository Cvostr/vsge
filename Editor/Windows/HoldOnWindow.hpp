#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <string>

namespace VSGEditor {
	class HoldOnWindow : public EditorWindow {
	private:
		std::string _task_description;
	public:
		void SetTaskDescription(const std::string& task);
		void OnDrawWindow();
	};
}