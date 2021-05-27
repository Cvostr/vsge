#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class SceneWindow : public EditorWindow {
	private:
		bool _entityRemoved;
	public:
		void OnDrawWindow();
		void DrawObjectTreeHierarchy(VSGE::Entity* entity);

		SceneWindow() {
			_entityRemoved = false;
		}
	};
}