#pragma once

#include "../Framebuffer.hpp"
#include "VulkanRenderpass.hpp"
#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanFramebuffer : public Framebuffer {
	private:
		VkFramebuffer mFramebuffer;
		uint32 mLayers;

		std::vector<VkImageView> Views;
		//ZSVulkanDescriptorSet* AttachmentsDescrSet;
	public:
		VulkanFramebuffer() : 
			mFramebuffer(VK_NULL_HANDLE),
			mLayers(1) {}

		VkFramebuffer GetFramebuffer() { return mFramebuffer; }

		void PushOutputAttachment(uint32_t Index);
		/// <summary>
		/// Add existing texture as attachment to framebuffer
		/// </summary>
		/// <param name="attachment">- pointer to texture</param>
		void AddAttachment(Texture* attachment);
		/// <summary>
		/// Create new texture and add it as attachment to framebuffer
		/// with size of framebuffer
		/// </summary>
		/// <param name="Format">- format of new texture</param>
		/// <param name="layers">- count of layers of new texture</param>
		void AddAttachment(TextureFormat Format = TextureFormat::FORMAT_RGBA, uint32 layers = 1);
		/// <summary>
		/// Create and set new depth attachment
		/// </summary>
		/// <param name="Format">- format of new depth texture</param>
		/// <param name="Layers">- count of layers of new depth texture</param>
		void AddDepth(TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8, uint32 Layers = 1);
		/// <summary>
		/// Change size property of framebuffer
		/// </summary>
		/// <param name="width">- new width of fb</param>
		/// <param name="height">- new height of fb</param>
		void SetSize(uint32 width, uint32 height);
		/// <summary>
		/// Create framebuffer gpu object from specified attachments
		/// </summary>
		bool Create(VulkanRenderPass* renderpass);
		/// <summary>
		/// Destroy created framebuffer
		/// </summary>
		void Destroy();
	};
}