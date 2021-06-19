#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {
	class ToolbarWindow : public EditorWindow {
	public:

		void PlayScene();
		void PauseScene();
		void StopScene();

		ToolbarWindow();

		void OnDrawWindow();
	};
}