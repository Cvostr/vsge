#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vector>

namespace VSGEditor {
	class StartWindow : public EditorWindow {
	private:
		std::vector<std::string> _projects;

		bool ReadProjects();
		void SaveProjectsList();

		void OpenProject(const std::string& path);
	public:

		StartWindow();

		void OnDrawWindow();
	};
}