#pragma once

#include "../GpuBuffer.hpp"
#include "../Texture.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanCommandBuffer.hpp"

namespace VSGE {

	class VulkanFramebuffer;

	class VulkanRenderPass : public IGpuObject {
	private:
		VkRenderPass mRenderPass;

		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		std::vector<VkAttachmentReference> mAttachmentReferences;
		VkAttachmentReference DepthDescriptionRef;
		bool mHasDepthAttachment;

		VkClearValue* mClearValues;
		int mClearValuesCount;
		VkExtent2D ClearExtent;

	public:

		VulkanRenderPass() : mHasDepthAttachment(false) {}

		/// <summary>
		/// Get vulkan descr to created render pass
		/// </summary>
		/// <returns></returns>
		VkRenderPass GetRenderPass() { return mRenderPass; }
		/// <summary>
		/// Try to create renderpass object
		/// </summary>
		/// <returns></returns>
		bool Create();
		/// <summary>
		/// Destroy renderpass
		/// </summary>
		void Destroy();

		void CmdBegin(VulkanCommandBuffer& cmdbuf, VulkanFramebuffer& framebuffer);

		void PushColorAttachment(VkFormat format, VkImageLayout Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		void PushColorAttachment(TextureFormat Format = TextureFormat::FORMAT_RGBA, VkImageLayout Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		void PushColorOutputAttachment();
		void PushDepthAttachment(TextureFormat Format = TextureFormat::FORMAT_DEPTH_24_STENCIL_8);
	};
}