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
		uint32 mColorAttachmentsCount;

		VkClearValue* mClearValues;
		int mClearValuesCount;
		VkExtent2D ClearExtent;

	public:

		VulkanRenderPass() : 
			mHasDepthAttachment(false),
			mRenderPass(VK_NULL_HANDLE),
			mColorAttachmentsCount(0),
			mClearValuesCount(0),
			mClearValues(nullptr)
		{
			ClearExtent.width = 0;
			ClearExtent.height = 0;
		}

		~VulkanRenderPass() {
			Destroy();
		}

		/// <summary>
		/// Get vulkan descr to created render pass
		/// </summary>
		/// <returns></returns>
		VkRenderPass GetRenderPass() { return mRenderPass; }
		/// <summary>
		/// Get count of color attachments on this renderpass
		/// </summary>
		/// <returns></returns>
		uint32 GetColorAttachmentsCount() { return mColorAttachmentsCount; }
		/// <summary>
		/// Try to create renderpass object
		/// </summary>
		/// <returns></returns>
		bool Create();
		/// <summary>
		/// Destroy renderpass
		/// </summary>
		void Destroy();
		/// <summary>
		/// Begin renderpass on specified command buffer
		/// </summary>
		/// <param name="cmdbuf"></param>
		/// <param name="framebuffer"></param>
		void CmdBegin(VulkanCommandBuffer& cmdbuf, VulkanFramebuffer& framebuffer);

		void SetClearSize(uint32 width, uint32 height);

		void PushColorAttachment(VkFormat format, VkImageLayout Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		void PushColorAttachment(TextureFormat Format = TextureFormat::FORMAT_RGBA, VkImageLayout Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		void PushColorOutputAttachment();
		void PushDepthAttachment(TextureFormat Format = TextureFormat::FORMAT_DEPTH_24_STENCIL_8);
	};
}