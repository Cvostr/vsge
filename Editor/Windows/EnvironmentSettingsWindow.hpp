#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {

	class EnvironmentSettingsWindow : public EditorWindow {
	private:
	public:

		EnvironmentSettingsWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}