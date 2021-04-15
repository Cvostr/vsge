#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {
	class BrowserWindow : public ImGuiWindow {
	public:
		void OnDrawWindow();
	};
}