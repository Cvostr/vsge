#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGEditor {
	class SceneViewWindow : public EditorWindow {
	private:
		bool _transform_gizmo_used;
		ImTextureID texture;
		ImTextureID gizmo_texture;
	public:
		void OnDrawWindow();

		void OnWindowResize();

		bool IsTransformGizmoUsed();

		SceneViewWindow();
	};
}