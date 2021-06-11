#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGEditor {
	class SceneViewWindow : public EditorWindow {
	private:
		bool _transform_gizmo_used;
	public:
		ImTextureID texture;
		void OnDrawWindow();

		void OnWindowResize();

		bool IsTransformGizmoUsed() {
			return _transform_gizmo_used;
		}

		SceneViewWindow() {
			_transform_gizmo_used = false;
			texture = VK_NULL_HANDLE;
		}
	};
}