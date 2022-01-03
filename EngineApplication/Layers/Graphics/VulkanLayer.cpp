#include "VulkanLayer.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

void VulkanLayer::OnAttach() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();

    _presenter = new VulkanPresenter;

    _imageAvailable = new VulkanSemaphore;
    _imageAvailable->Create();
 
    VulkanSwapChain* swc = vk->GetSwapChain();
    VkExtent2D extent = swc->GetExtent();
    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
    renderer->ResizeOutput(extent.width, extent.height);

    renderer->SetBeginSemaphore(_imageAvailable);
}
void VulkanLayer::OnUpdate() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    uint32_t imageIndex;
    VkResult imageResult = AcquireNextImage(*_imageAvailable, imageIndex);

    _presenter->Update(imageResult);

    if (_presenter->IsRecreated())
        UpdatePresentingTexture();

    if (!_presenter->IsRecreated()) {
        VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
        renderer->GetFinalPass()->SetFramebuffer(_presenter->GetFramebuffer(imageIndex));
        renderer->DrawScene(nullptr);
        VulkanPresent(*renderer->GetEndSemaphore(), imageIndex);
    }
}

void VulkanLayer::OnDetach() {

}

void VulkanLayer::UpdatePresentingTexture() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VulkanSwapChain* swc = vk->GetSwapChain();
    VkExtent2D new_extent = swc->GetExtent();
    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();

    renderer->ResizeOutput(new_extent.width, new_extent.height);
    renderer->GetFinalPass()->Resize(new_extent.width, new_extent.height);
}