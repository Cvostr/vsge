#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {

	class ProjectBuildingWindow : public EditorWindow {
	private:

	public:

		ProjectBuildingWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}