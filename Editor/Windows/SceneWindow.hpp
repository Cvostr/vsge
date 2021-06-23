#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class SceneWindow : public EditorWindow {
	private:
		int _drawn_entities;
		bool _entityRemoved;
	public:
		void OnDrawWindow();
		void DrawEntityTreeHierarchy(VSGE::Entity* entity);

		SceneWindow() {
			_drawn_entities = 0;
			_entityRemoved = false;
		}
	};
}