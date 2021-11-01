#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/ResourceReference.hpp>

namespace VSGEditor {

	class ProjectSettingsWindow : public EditorWindow {
	private:
		std::string _project_name;
		VSGE::ResourceReference _main_scene;

		void Save();
	public:

		ProjectSettingsWindow();

		void LoadSettings();
		void OnDrawWindow();
	};
}