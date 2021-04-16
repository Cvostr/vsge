#pragma once

#include "../GpuBuffer.hpp"
#include "../Texture.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanRenderPass : public IGpuObject {
	private:
		VkRenderPass mRenderPass;

		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		std::vector<VkAttachmentReference> mAttachmentReferences;
		VkAttachmentReference DepthDescriptionRef;

		VkClearValue* mClearValues;
		int mClearValuesCount;
		VkExtent2D ClearExtent;
	public:
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

		void PushColorAttachment(TextureFormat Format = TextureFormat::FORMAT_RGBA);
		void PushColorOutputAttachment();
		void PushDepthAttachment(TextureFormat Format = TextureFormat::FORMAT_DEPTH_24_STENCIL_8);
	};
}