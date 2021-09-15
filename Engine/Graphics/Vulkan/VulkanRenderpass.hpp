#pragma once

#include "../GpuBuffer.hpp"
#include "../Texture.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanCommandBuffer.hpp"
#include <Math/Color.hpp>

namespace VSGE {

	class VulkanFramebuffer;

	class VulkanRenderPass : public IGpuObject {
	private:
		VkRenderPass _renderPass;

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
			_renderPass(VK_NULL_HANDLE),
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
		VkRenderPass GetRenderPass() { return _renderPass; }
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
		void SetClearColor(uint32 attachment_id, const Color& color);

		void PushColorAttachment(VkFormat format, VkImageLayout Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		void PushColorAttachment(TextureFormat Format = TextureFormat::FORMAT_RGBA, VkImageLayout Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		void PushColorOutputAttachment();
		void PushDepthAttachment(TextureFormat Format = TextureFormat::FORMAT_DEPTH_24_STENCIL_8);
		void PushDepthAttachment(VkFormat format = VK_FORMAT_D24_UNORM_S8_UINT);
	};
}