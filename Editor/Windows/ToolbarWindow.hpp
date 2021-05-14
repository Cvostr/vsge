#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {
	class ToolbarWindow : public EditorWindow {
	public:

		ToolbarWindow();

		void OnDrawWindow();
	};
}