#pragma once

#include <Engine/ApplicationLayer.hpp>

#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>

#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>

#include "../Base/ImGuiWindow.hpp"
#include "../Base/ImGuiMenu.hpp"

namespace VSGEditor {
	class ImGuiLayer : public VSGE::IApplicationLayer {
	private:
		static ImGuiLayer* _this;

		ImGuiIO io;

		tEditorWindowList mWindows;
		tImGuiMenuList mMenus;

		VSGE::VulkanDescriptorPool imgui_pool;
		VSGE::VulkanRenderPass imgui_rpass;
		VSGE::VulkanFramebuffer imgui_fb;

		VSGE::VulkanSemaphore imageAvailable;
		VSGE::VulkanSemaphore presentBegin;

		VSGE::VulkanCommandPool cmdpool;
		VSGE::VulkanCommandBuffer cmdbuf;
	public:

		ImGuiLayer() {
			_this = this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		void OnSDL2Event(void* event_ptr);

		void AddWindow(EditorWindow* window);
		void AddMenu(ImGuiMenu* menu);

		void VulkanComputeAndPresent(ImDrawData* draw_data);

		static ImGuiLayer* Get() {
			return _this;
		}
	};
}