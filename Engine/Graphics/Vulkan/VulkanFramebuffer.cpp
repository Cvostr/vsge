#include "VulkanFramebuffer.hpp"
#include "VulkanRAPI.hpp"
#include "VulkanTexture.hpp"

using namespace VSGE;

VulkanFramebuffer::VulkanFramebuffer() :
	_framebuffer(VK_NULL_HANDLE),
	_layers(1) 
{}


VulkanFramebuffer::~VulkanFramebuffer() {
	Destroy();
}

void VulkanFramebuffer::AddAttachment(Texture* attachment) {
	if (attachment->IsRenderTarget()) {
		_attachments.push_back(attachment);
		_views.push_back(((VulkanTexture*)attachment)->GetImageView());
	}
}

void VulkanFramebuffer::AddAttachment(TextureFormat Format, uint32 layers) {
	VulkanTexture* new_attachment = new VulkanTexture;
	new_attachment->SetRenderTargetFlag(true);
	new_attachment->Create(mWidth, mHeight, Format, layers, 1);
	new_attachment->CreateImageView();
	
	AddAttachment(new_attachment);
}

void VulkanFramebuffer::AddDepth(TextureFormat Format, uint32 Layers) {
	if (Format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8 || Format == TextureFormat::FORMAT_DEPTH_32) {
		VulkanTexture* new_attachment = new VulkanTexture;
		new_attachment->SetRenderTargetFlag(true);
		new_attachment->Create(mWidth, mHeight, Format, Layers, 1);
		new_attachment->CreateImageView();

		mDepthAttachment = new_attachment;
		_views.push_back(((VulkanTexture*)new_attachment)->GetImageView());
	}
}

void VulkanFramebuffer::PushOutputAttachment(uint32_t Index) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanSwapChain* swapchain = vulkan_rapi->GetSwapChain();

	Framebuffer::SetSize(swapchain->GetExtent().width, swapchain->GetExtent().height);
	
	_views.push_back(vulkan_rapi->GetSwapChain()->GetImageViewAtIndex(Index));
}

void VulkanFramebuffer::SetSize(uint32 width, uint32 height) {
	if (mWidth != width || mHeight != height) {
		Framebuffer::SetSize(width, height);
	}
}

void VulkanFramebuffer::SetLayersCount(uint32 layers) {
	_layers = layers;
}

uint32 VulkanFramebuffer::GetLayersCount() {
	return _layers;
}

bool VulkanFramebuffer::Create(VulkanRenderPass* renderpass) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanDevice* device = vulkan_rapi->GetDevice();

	if (!mCreated) {
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(_views.size());
		framebufferInfo.pAttachments = _views.data();
		framebufferInfo.width = mWidth;
		framebufferInfo.height = mHeight;
		framebufferInfo.layers = _layers;
		//Try to create framebuffer
		if (vkCreateFramebuffer(device->getVkDevice(), &framebufferInfo, nullptr, &_framebuffer) != VK_SUCCESS) {
			return false;
		}
	
		mCreated = true;

	}
	return true;
}

void VulkanFramebuffer::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
		VulkanDevice* device = vulkan_rapi->GetDevice();

		vkDestroyFramebuffer(device->getVkDevice(), _framebuffer, nullptr);
		_views.clear();
		_layers = 1;
		mCreated = false;
	}
}