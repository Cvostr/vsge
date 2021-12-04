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
		//project settings
		std::string _project_name;
		VSGE::ResourceReference _main_scene;
		//Application settings
		std::string _application_name;
		int _version;
		std::string _version_string;

		void Save();

		void DrawProjectSettings();
		void DrawApplicationSettings();
	public:

		ProjectSettingsWindow();

		void SetDrawMode(DrawMode mode);

		void LoadSettings();
		void OnDrawWindow();
	};
}