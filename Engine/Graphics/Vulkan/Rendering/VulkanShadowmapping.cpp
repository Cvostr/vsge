#include "VulkanShadowmapping.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanPipeline.hpp"

using namespace VSGE;

VulkanShadowmapping::VulkanShadowmapping() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_shadowmapBeginSemaphore = new VulkanSemaphore;
	_shadowmapBeginSemaphore->Create();

	_shadowmapEndSemaphore = new VulkanSemaphore;
	_shadowmapEndSemaphore->Create();

	_shadowmapRenderPass = new VulkanRenderPass;
	_shadowmapRenderPass->SetClearSize(2048, 2048);
	_shadowmapRenderPass->PushDepthAttachment(FORMAT_DEPTH_32);
	_shadowmapRenderPass->Create();

	_shadowmapCmdPool->Create(device->GetGraphicsQueueFamilyIndex());

	_shadowmapPipeline = new VulkanPipeline;
}
VulkanShadowmapping::~VulkanShadowmapping() {
	delete _shadowmapBeginSemaphore;
	delete _shadowmapEndSemaphore;

	delete _shadowmapRenderPass;

	delete _shadowmapCmdPool;
}