#pragma once

#include "../Base/ImGuiWindow.hpp"

namespace VSGEditor {
	class PlayerViewWindow : public EditorWindow {
	private:
		ImTextureID texture;
	public:
		void OnDrawWindow();

		void OnWindowResize();

		PlayerViewWindow();
	};
}