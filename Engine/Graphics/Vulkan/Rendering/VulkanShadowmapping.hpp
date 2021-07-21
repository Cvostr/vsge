#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanCommandBuffer.hpp"
#include "../VulkanSynchronization.hpp"

namespace VSGE {

	class VulkanShadowCaster {
	public:
		VulkanFramebuffer* _framebuffer;
		VulkanCommandBuffer* _cmdbuf;

		VulkanShadowCaster() {
			_framebuffer = new VulkanFramebuffer;
			_cmdbuf = new VulkanCommandBuffer;
		}

		~VulkanShadowCaster() {
			delete _framebuffer;
			delete _cmdbuf;
		}
	};

	class VulkanShadowmapping {
	private:
		VulkanSemaphore* _shadowmapBeginSemaphore;
		VulkanSemaphore* _shadowmapEndSemaphore;

		VulkanRenderPass* _shadowmapRenderPass;

		VulkanCommandPool* _shadowmapCmdPool;

		VulkanPipeline* _shadowmapPipeline;
	public:

		VulkanShadowmapping();
		~VulkanShadowmapping();
	};
}