#include "VulkanFramebuffer.hpp"
#include "VulkanRAPI.hpp"
#include "VulkanTexture.hpp"

using namespace VSGE;

void VulkanFramebuffer::AddAttachment(Texture* attachment) {
	if (attachment->IsRenderTarget()) {
		mAttachments.push_back(attachment);
		Views.push_back(((VulkanTexture*)attachment)->GetImageView());
	}
}

void VulkanFramebuffer::AddAttachment(TextureFormat Format, uint32 layers) {
	VulkanTexture* new_attachment = new VulkanTexture;
	new_attachment->SetRenderTargetFlag(true);
	new_attachment->Create(mWidth, mHeight, Format, layers);
	
	AddAttachment(new_attachment);
}

void VulkanFramebuffer::AddDepth(TextureFormat Format, uint32 Layers) {
	if (Format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8 || Format == TextureFormat::FORMAT_DEPTH_32) {
		VulkanTexture* new_attachment = new VulkanTexture;
		new_attachment->SetRenderTargetFlag(true);
		new_attachment->Create(mWidth, mHeight, Format, Layers);

		mDepthAttachment = new_attachment;
		Views.push_back(((VulkanTexture*)new_attachment)->GetImageView());
	}
}

void VulkanFramebuffer::PushOutputAttachment(uint32_t Index) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	Views.push_back(vulkan_rapi->GetSwapChain()->GetImageViewAtIndex(Index));
}

void VulkanFramebuffer::SetSize(uint32 width, uint32 height) {
	Framebuffer::SetSize(width, height);
}

bool VulkanFramebuffer::Create(VulkanRenderPass* renderpass) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanDevice* device = vulkan_rapi->GetDevice();

	if (!mCreated) {
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(Views.size());
		framebufferInfo.pAttachments = Views.data();
		framebufferInfo.width = mWidth;
		framebufferInfo.height = mHeight;
		framebufferInfo.layers = mLayers;
		//Try to create framebuffer
		if (vkCreateFramebuffer(device->getVkDevice(), &framebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
			return false;
		}
		//Store picked renderpass
		//mPickedRenderPass = renderpass;


		//AttachmentsDescrSet = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE);
		//for (unsigned int attachment_i = 0; attachment_i < Views.size(); attachment_i++)
		//	AttachmentsDescrSet->pushImageSampler(attachment_i);

		//AttachmentsDescrSet->getDescriptorSetLayout();

		//for (unsigned int attachment_i = 0; attachment_i < Views.size(); attachment_i++)
		//	AttachmentsDescrSet->setTexture(attachment_i, Views[attachment_i], game_data->vk_main->mDefaultTextureSampler);

		mCreated = true;

	}
	return true;
}

void VulkanFramebuffer::Destroy() {
	if (mCreated) {

	}
}