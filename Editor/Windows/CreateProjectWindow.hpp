#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {

	class CreateProjectWindow : public EditorWindow {
	private:
		std::string project_name;
		std::string project_directory;
	public:

		CreateProjectWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}