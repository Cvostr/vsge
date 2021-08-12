#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {

	class ProjectSettingsWindow : public EditorWindow {
	private:
	public:

		ProjectSettingsWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}