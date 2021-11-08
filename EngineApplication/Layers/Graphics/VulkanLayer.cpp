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

    _copy_shader = new VulkanShader;
    _copy_shader->AddShaderFromFile("copy.vert", SHADER_STAGE_VERTEX);
    _copy_shader->AddShaderFromFile("copy.frag", SHADER_STAGE_FRAGMENT);

    _presenter = new VulkanPresenter;

    _pool = new VulkanDescriptorPool;
    _pool->SetDescriptorSetsCount(1);
    _pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
    _pool->Create();

    _set = new VulkanDescriptorSet;
    _set->SetDescriptorPool(_pool);
    _set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    _set->Create();

    _output_pipeline_layout = new VulkanPipelineLayout;
    _output_pipeline_layout->PushDescriptorSet(_set);
    _output_pipeline_layout->Create();

    VertexLayout _vertexLayout;
    _vertexLayout.AddBinding(sizeof(Vertex));
    _vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
    _vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

    output_pipeline = new VulkanPipeline;
    output_pipeline->SetDepthTest(false);
    output_pipeline->Create(_copy_shader, _presenter->GetRenderPass(), _vertexLayout, _output_pipeline_layout);

    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();

    _set->WriteDescriptorImage(0, (VulkanTexture*)renderer->GetOutputTexture(), renderer->GetAttachmentSampler());

    renderer->SetBeginSemaphore(_imageAvailable);
}
void VulkanLayer::OnUpdate() {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    uint32_t _imageIndex;
    VkResult imageResult = AcquireNextImage(*_imageAvailable, _imageIndex);
    _recreated = false;
    //Check, if swapchain is no more suitable
    /*if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
        //Swapchain is no more suitable
        vkDeviceWaitIdle(vk->GetDevice()->getVkDevice());
        //Recreate swapchain
        vk->GetSwapChain()->Destroy();
        vk->GetSwapChain()->initSwapchain(vk->GetDevice());

        _recreated = true;
    }*/
    //if (!_recreated)
    //    VulkanGraphicsSubmit(cmdbuf, *_imageAvailable, *endSemaphore);

    RecordCmdbuf(_imageIndex);

	VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
	renderer->DrawScene(nullptr);
    VulkanGraphicsSubmit(*_cmdbuf, *renderer->GetEndSemaphore(), *_presentBegin);
    VulkanPresent(*_presentBegin, _imageIndex);
}

void VulkanLayer::OnDetach() {

}

void VulkanLayer::RecordCmdbuf(uint32 index) {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VulkanSwapChain* swc = vk->GetSwapChain();
    
    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();
    VulkanTexture* output = (VulkanTexture*)renderer->GetOutputTexture();

    VulkanMesh* mesh = VulkanRenderer::Get()->GetScreenMesh();

    _cmdbuf->Begin();
    _presenter->GetRenderPass()->CmdBegin(*_cmdbuf, *_presenter->GetFramebuffer(index));
    _cmdbuf->BindPipeline(*output_pipeline);
    _cmdbuf->SetViewport(0, 0, 1280, 720);
    _cmdbuf->BindDescriptorSets(*_output_pipeline_layout, 0, 1, _set);
    _cmdbuf->BindMesh(*mesh, 0);
    _cmdbuf->DrawIndexed(6);
    _cmdbuf->EndRenderPass();
    _cmdbuf->End();
}