#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

namespace VSGETest {

	class VulkanRectTestLayer : public VSGE::IApplicationLayer {
	private:
		VSGE::VulkanSemaphore imageAvailable;
		VSGE::VulkanSemaphore presentBegin;
		VSGE::VulkanCommandBuffer* cmdbuf;
	public:

		VulkanRectTestLayer() {
			
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

	};
}