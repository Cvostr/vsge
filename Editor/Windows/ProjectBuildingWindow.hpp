#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <ProjectBuild/ResourcePacker.hpp>

namespace VSGEditor {

	class ProjectBuildingWindow : public EditorWindow {
	private:
		ResourcePacker* _packer;
		bool _building;
		void Build();
		void DrawPreBuildingInterface();
		void DrawBuildingInterface();
	public:

		ProjectBuildingWindow();
		~ProjectBuildingWindow();

		void OnDrawWindow();
	};
}