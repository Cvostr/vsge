#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {

	class CreateProjectWindow : public EditorWindow {
	private:
	public:

		CreateProjectWindow() {
			_showCloseCross = true;
		}

		void OnDrawWindow();
	};
}