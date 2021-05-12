#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGEditor {
	class SceneViewWindow : public EditorWindow {
	public:
		ImTextureID texture;
		void OnDrawWindow();

		SceneViewWindow() {
			texture = VK_NULL_HANDLE;
		}
	};
}