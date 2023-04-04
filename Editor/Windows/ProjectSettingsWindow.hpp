#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/ResourceReference.hpp>

namespace VSGEditor {

	enum DrawMode {
		MODE_PROJECT = 1,
		MODE_APP
	};

	class ProjectSettingsWindow : public EditorWindow {
	private:
		DrawMode _draw_mode;

		void Save();

		void DrawProjectSettings();
		void DrawApplicationSettings();
	public:

		ProjectSettingsWindow();

		void SetDrawMode(DrawMode mode);

		void OnDrawWindow();
	};
}