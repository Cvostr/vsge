#pragma once

#include <vector>

namespace VSGEditor {
	class ImGuiMenu {
	public:
		virtual void OnDrawMenu() = 0;
	};

	typedef std::vector<ImGuiMenu*> tImGuiMenuList;
}