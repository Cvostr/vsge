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

void VulkanFramebuffer::AddAttachment(TextureFormat Format, uint32 layers, bool cubemap) {
	VulkanTexture* new_attachment = new VulkanTexture;
	new_attachment->SetCubemap(cubemap);
	new_attachment->SetRenderTargetFlag(true);
	new_attachment->Create(_width, _height, Format, layers, 1);
	new_attachment->CreateImageView();
	
	AddAttachment(new_attachment);
}

void VulkanFramebuffer::AddDepth(TextureFormat format, uint32 layers, bool cubemap) {	
	if (format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8 || format == TextureFormat::FORMAT_DEPTH_32) {
		VulkanTexture* new_attachment = new VulkanTexture;
		new_attachment->SetCubemap(cubemap);
		new_attachment->SetRenderTargetFlag(true);
		new_attachment->Create(_width, _height, format, layers, 1);
		new_attachment->CreateImageView();

		_depthAttachment = new_attachment;
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
	if (_width != width || _height != height) {
		Framebuffer::SetSize(width, height);

		if (IsCreated()) {
			uint32 layers = _layers;
			Destroy();

			for (auto attachment : _attachments) {
				attachment->Resize(width, height);
				attachment->CreateImageView();
				_views.push_back(((VulkanTexture*)attachment)->GetImageView());
			}

			if (_depthAttachment) {
				_depthAttachment->Resize(width, height);
				_depthAttachment->CreateImageView();
				_views.push_back(((VulkanTexture*)_depthAttachment)->GetImageView());
			}

			_layers = layers;
			Create(_renderpass);
		}
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
	_renderpass = renderpass;

	if (!mCreated) {
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(_views.size());
		framebufferInfo.pAttachments = _views.data();
		framebufferInfo.width = _width;
		framebufferInfo.height = _height;
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