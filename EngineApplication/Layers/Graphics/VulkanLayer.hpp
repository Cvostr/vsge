#pragma once

#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Engine/ApplicationLayer.hpp>

namespace VSGE {
	class VulkanLayer : public IApplicationLayer {
	private:
		bool _recreated;

		VulkanCommandPool* _cmdpool;
		VulkanCommandBuffer* _cmdbuf;

		VulkanSemaphore* _imageAvailable;
		VulkanSemaphore* _presentBegin;

		void RecordCmdbuf();
	public:
		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}