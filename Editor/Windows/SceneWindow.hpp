#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class SceneWindow : public EditorWindow {
	public:
		void OnDrawWindow();
		void DrawObjectTreeHierarchy(VSGE::Entity* entity);
	};
}