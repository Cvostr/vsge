#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanDescriptors.hpp"

namespace VSGE {
	class VulkanGBufferRenderer {
	private:
		VulkanFramebuffer* _gbuffer_fb;
		VulkanRenderPass* _gbuffer_renderpass;

		uint32 _fb_width;
		uint32 _fb_height;
	public:

		VulkanGBufferRenderer();
		~VulkanGBufferRenderer();

		void CreateFramebuffer();
	};
}