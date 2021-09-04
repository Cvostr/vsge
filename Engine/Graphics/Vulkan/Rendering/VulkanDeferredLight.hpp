#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanDescriptors.hpp"

namespace VSGE {
	class VulkanDefferedLight {
	private:
		VulkanFramebuffer* _deferred_fb;
		VulkanRenderPass* _deferred_rp;

		VulkanDescriptorPool* _deferred_pool;
		VulkanDescriptorSet* _deferred_descriptor;

		uint32 _fb_width;
		uint32 _fb_height;
	public:
		VulkanDefferedLight();
		~VulkanDefferedLight();

		void CreateFramebuffer();
		void CreateDescriptorSet();
	};
}