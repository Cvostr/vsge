#include "VulkanGBufferRenderer.hpp"
#include "../VulkanRAPI.hpp"

using namespace VSGE;

VulkanGBufferRenderer::VulkanGBufferRenderer() {
	_fb_width = 1280;
	_fb_height = 720;
}
VulkanGBufferRenderer::~VulkanGBufferRenderer() {
	SAFE_RELEASE(_gbuffer_fb)
	SAFE_RELEASE(_gbuffer_renderpass)
}

void VulkanGBufferRenderer::CreateFramebuffer() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_gbuffer_renderpass = new VulkanRenderPass;
	_gbuffer_renderpass->SetClearSize(_fb_width, _fb_height);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushDepthAttachment(device->GetSuitableDepthFormat());
	_gbuffer_renderpass->Create();

	_gbuffer_fb = new VulkanFramebuffer;
	_gbuffer_fb->SetSize(_fb_width, _fb_height);
	_gbuffer_fb->AddAttachment(FORMAT_RGBA); //Color
	_gbuffer_fb->AddAttachment(FORMAT_RGBA16F); //Normal
	_gbuffer_fb->AddAttachment(FORMAT_RGBA16F); //Position
	_gbuffer_fb->AddAttachment(FORMAT_RGBA); //Material
	_gbuffer_fb->AddDepth(GetTextureFormat(device->GetSuitableDepthFormat()));
	_gbuffer_fb->Create(_gbuffer_renderpass);
}