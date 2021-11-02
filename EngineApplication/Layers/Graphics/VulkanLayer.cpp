#include "VulkanLayer.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

void VulkanLayer::OnAttach() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();

    _cmdpool = new VulkanCommandPool;
    _cmdpool->Create(vk->GetDevice()->GetGraphicsQueueFamilyIndex());

    _cmdbuf = new VulkanCommandBuffer;
    _cmdbuf->Create(_cmdpool);

    _imageAvailable = new VulkanSemaphore;
    _imageAvailable->Create();

    _presentBegin = new VulkanSemaphore;
    _presentBegin->Create();
}
void VulkanLayer::OnUpdate() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    uint32_t _imageIndex;
    VkResult imageResult = AcquireNextImage(*_imageAvailable, _imageIndex);
    _imageIndex = 0;
    _recreated = false;
    //Check, if swapchain is no more suitable
    if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
        //Swapchain is no more suitable
        vkDeviceWaitIdle(vk->GetDevice()->getVkDevice());
        //Recreate swapchain
        vk->GetSwapChain()->Destroy();
        vk->GetSwapChain()->initSwapchain(vk->GetDevice());

        _recreated = true;
    }
    //if (!_recreated)
    //    VulkanGraphicsSubmit(cmdbuf, *_imageAvailable, *endSemaphore);

	VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
	renderer->DrawScene(nullptr);
    //VulkanGraphicsSubmit(*_cmdbuf, *_imageAvailablem);
    VulkanPresent(*renderer->GetEndSemaphore(), 0);
}

void VulkanLayer::OnDetach() {

}

void VulkanLayer::RecordCmdbuf() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VulkanSwapChain* swc = vk->GetSwapChain();
    
    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
    VulkanTexture* output = (VulkanTexture*)renderer->GetOutputTexture();

    _cmdbuf->Begin();

    //VkImageLayout old = _output->GetImageLayout();
    //_output->CmdChangeLayout(_cmdbuf, old, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkImageCopy copy = {};
    VkImageSubresourceLayers src = {};
    src.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    src.mipLevel = 0;
    src.baseArrayLayer = 0;
    src.layerCount = 1;

    VkImageSubresourceLayers dst = src;
    dst.baseArrayLayer = 0;

    copy.srcSubresource = src;
    copy.dstSubresource = dst;

    VkOffset3D offset;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;

    copy.srcOffset = offset;
    copy.dstOffset = offset;
    copy.extent = { output->GetWidth(), output->GetHeight(), 1 };

    output->CmdChangeLayout(_cmdbuf, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    VkImage image = output->GetImage();
    vkCmdCopyImage(
        _cmdbuf->GetCommandBuffer(),
        image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        swc->GetImageAtIndex(0),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copy);

    output->CmdChangeLayout(_cmdbuf, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _cmdbuf->End();
}