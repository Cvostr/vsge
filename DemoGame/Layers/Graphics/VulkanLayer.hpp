#pragma once

#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanFramebuffer.hpp>
#include <Graphics/Vulkan/VulkanRenderpass.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Engine/ApplicationLayer.hpp>
#include <Graphics/Vulkan/VulkanPresenter.hpp>

namespace VSGE {
	class VulkanLayer : public IApplicationLayer {
	private:
		VulkanPresenter* _presenter;

		VulkanSemaphore* _imageAvailable;

		void UpdatePresentingTexture();
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}