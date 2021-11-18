#pragma once

#include <Engine/ApplicationLayer.hpp>

#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanPresenter.hpp>
#include <Windows/HoldOnWindow.hpp>

#include "../Base/ImGuiWindow.hpp"
#include "../Base/ImGuiMenu.hpp"

namespace VSGEditor {
	class ImGuiLayer : public VSGE::IApplicationLayer {
	private:
		static ImGuiLayer* _this;

		ImGuiIO io;

		tEditorWindowList mWindows;
		tImGuiMenuList mMenus;
		HoldOnWindow* mHoldOnWindow;
		bool _draw_windows;


		VSGE::VulkanDescriptorPool imgui_pool;
		VSGE::VulkanPresenter* presenter;

		VSGE::VulkanSemaphore imageAvailable;
		VSGE::VulkanSemaphore presentBegin;

		VSGE::VulkanCommandPool cmdpool;
		VSGE::VulkanCommandBuffer cmdbuf;
	public:

		ImGuiLayer();

		void DrawDockWindow();

		void RegroupWindows();

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		void OnSDL2Event(void* event_ptr);
		void OnEvent(const VSGE::IEvent& event);

		void AddWindow(EditorWindow* window);
		void RemoveWindow(EditorWindow* window);
		void SetDrawWindows(bool draw_windows);
		HoldOnWindow* GetHoldOnWindow();

		void OnWindowResize(const VSGE::EventWindowResized& wr);

		template<typename T>
		T* GetWindow() {
			for (auto window : mWindows) {
				if (typeid(*window) == typeid(T)) {
					return static_cast<T*>(window);
				}
			}
			return nullptr;
		}

		template<typename T>
		T* GetMenu() {
			for (auto menu : mMenus) {
				if (typeid(*menu) == typeid(T)) {
					return static_cast<T*>(menu);
				}
			}
			return nullptr;
		}

		void AddMenu(ImGuiMenu* menu);

		uint32 VulkanRender(ImDrawData* draw_data, VSGE::VulkanSemaphore* endSemaphore);
		void VulkanRecordCmdBuf(ImDrawData* draw_data, uint32 image_index = 0);

		static ImGuiLayer* Get() {
			return _this;
		}
	};
}