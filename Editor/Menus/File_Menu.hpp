#pragma once

#include "../Base/ImGuiMenu.hpp"

namespace VSGEditor {
	class File_Menu : public ImGuiMenu {
	public:
		void OnSave();
		void OnDrawMenu();
	};
}