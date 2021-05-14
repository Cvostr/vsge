#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGEditor {
	class SceneViewWindow : public EditorWindow {
	public:
		ImTextureID texture;
		void OnDrawWindow();

		void OnWindowResize();

		void Regroup(uint32 width, uint32 height);

		SceneViewWindow() {
			texture = VK_NULL_HANDLE;
		}
	};
}