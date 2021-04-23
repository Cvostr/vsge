#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanSampler.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>

namespace VSGETest {

	class VulkanRectTestLayer : public VSGE::IApplicationLayer {
	private:
		VSGE::VulkanSemaphore imageAvailable;
		VSGE::VulkanSemaphore presentBegin;
		VSGE::VulkanCommandBuffer* cmdbuf;

		VSGE::VulkanTexture test_texture;
		VSGE::VulkanTexture test_texture_bc;
		VSGE::VulkanSampler sampler;

		VSGE::VulkanBuffer* uniformBuffer;
		float scale_factor;
	public:

		VulkanRectTestLayer() {
			
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();
		void OnWindowEvent(VSGE::IWindowEvent& event);
	};
}