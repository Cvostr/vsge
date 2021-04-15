#pragma once

#include <Engine/ApplicationLayer.hpp>

#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>

#include "../Base/ImGuiWindow.hpp"
#include "../Base/ImGuiMenu.hpp"

namespace VSGEditor {
	class ImGuiLayer : public VSGE::IApplicationLayer {
	private:
		static ImGuiLayer* _this;

		ImGuiIO io;

		tImGuiWindowList mWindows;
		tImGuiMenuList mMenus;
	public:

		ImGuiLayer() {
			_this = this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		void OnSDL2Event(void* event_ptr);

		void AddWindow(ImGuiWindow* window);
		void AddMenu(ImGuiMenu* menu);

		static ImGuiLayer* Get() {
			return _this;
		}
	};
}