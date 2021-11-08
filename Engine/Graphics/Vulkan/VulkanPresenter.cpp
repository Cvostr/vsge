#include "VulkanPresenter.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanPresenter::VulkanPresenter() {
	Create();
}
VulkanPresenter::~VulkanPresenter() {
	Destroy();
}

VulkanRenderPass* VulkanPresenter::GetRenderPass() {
	return _renderpass;
}
VulkanFramebuffer* VulkanPresenter::GetFramebuffer(uint32 index) {
	return _fbs[index];
}

void VulkanPresenter::Create() {
	VulkanRAPI* rapi = VulkanRAPI::Get();
	VulkanSwapChain* swc = rapi->GetSwapChain();
	uint32 swc_imgs_count = swc->GetSwapChainImagesCount();

	_renderpass = new VulkanRenderPass;
	_renderpass->PushColorOutputAttachment();
	_renderpass->Create();

	for (uint32 i = 0; i < swc_imgs_count; i++) {
		VulkanFramebuffer* fb = new VulkanFramebuffer;
		fb->PushOutputAttachment(i);
		fb->Create(_renderpass);
		_fbs.push_back(fb);
	}
}

void VulkanPresenter::Recreate() {
	Destroy();
	Create();
}

void VulkanPresenter::Destroy() {
	SAFE_RELEASE(_renderpass);
	for (uint32 i = 0; i < _fbs.size(); i ++) {
		SAFE_RELEASE(_fbs[i]);
	}
	_fbs.clear();
}