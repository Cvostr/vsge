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

		void DrawGeneralSettings();
		void DrawSceneView();
		void DrawBuilding();
		void DrawMonoSettings();
	public:

		EditorSettingsWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}