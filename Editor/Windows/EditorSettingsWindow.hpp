#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {

	enum SettingsMode {
		Settings_General,
		Settings_SceneView,
		Settings_Building,
		Settings_Mono
	};

	class EditorSettingsWindow : public EditorWindow {
	private:
		SettingsMode _mode;
	public:

		EditorSettingsWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}