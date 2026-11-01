#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <ProjectBuild/ResourcePacker.hpp>

namespace VSGEditor {

	class ProjectBuildingWindow : public EditorWindow {
	private:

		ResourcePacker* _packer;
		bool _building;
		std::string outputDirPath;

		void Build();
		void DrawPreBuildingInterface();
		void DrawBuildingInterface();

		std::string GetDefaultOutputDirectory();
	public:

		ProjectBuildingWindow();
		~ProjectBuildingWindow();

		void OnDrawWindow();
	};
}