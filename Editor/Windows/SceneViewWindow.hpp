#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {
	class SceneViewWindow : public ImGuiWindow {
	public:
		void OnDrawWindow();
	};
}