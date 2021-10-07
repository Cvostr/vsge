#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Resources/ResourceReference.hpp>

namespace VSGEditor {

	class ProjectSettingsWindow : public EditorWindow {
	private:
		std::string _project_name;
		VSGE::ResourceReference _main_scene;
	public:

		ProjectSettingsWindow() {
			_showCloseCross = true;
			_main_scene.SetResourceType(VSGE::RESOURCE_TYPE_SCENE);
		}

		void OnDrawWindow();
	};
}