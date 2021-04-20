#include "VulkanFramebuffer.hpp"
#include <Core/VarTypes/Base.hpp>
#include "VulkanRAPI.hpp"
#include "VulkanTexture.hpp"

using namespace VSGE;

bool VulkanRenderPass::Create() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    //Create SubPass
    //This collects attachment references
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount =
        static_cast<uint32_t>(mAttachmentReferences.size());
    subpass.pColorAttachments = mAttachmentReferences.data();
    if (mHasDepthAttachment)
        subpass.pDepthStencilAttachment = &DepthDescriptionRef;

    VkSubpassDependency dependencies[2];
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    //RENDER PASS
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(mAttachmentDescriptions.size());
    renderPassInfo.pAttachments = mAttachmentDescriptions.data();
    //Set subpass
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    //Set dependencies
    renderPassInfo.dependencyCount = 2;
    renderPassInfo.pDependencies = dependencies;


    if (vkCreateRenderPass(device->getVkDevice(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
        return false;
    }

    mClearValuesCount = static_cast<int>(mAttachmentReferences.size() + mHasDepthAttachment);
    mClearValues = new VkClearValue[mClearValuesCount];

    for (int i = 0; i < mAttachmentReferences.size(); i++)
        mClearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    if (mHasDepthAttachment)
        mClearValues[mClearValuesCount - 1].depthStencil = { 1.0f, 0 };
    //Set created flag to true
    mCreated = true;
    return true;
}

void VulkanRenderPass::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan = VulkanRAPI::Get();
        VulkanDevice* device = vulkan->GetDevice();

        vkDestroyRenderPass(device->getVkDevice(), mRenderPass, nullptr);

        delete[] mClearValues;

        mColorAttachmentsCount = 0;

        mCreated = false;
    }
}

void VulkanRenderPass::PushColorAttachment(VkFormat format, VkImageLayout Layout) {
    VkImageLayout imgLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if (Layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        imgLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    else
        imgLayout = Layout;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = imgLayout;
    //Push new attachment description
    mAttachmentDescriptions.push_back(colorAttachment);


    //Create Attachment Reference for Depth Attachment
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment =
        static_cast<uint32>(mAttachmentReferences.size());
    colorAttachmentRef.layout = Layout;
    //Push new attachment reference
    mAttachmentReferences.push_back(colorAttachmentRef);

    mColorAttachmentsCount++;
}

void VulkanRenderPass::PushColorAttachment(TextureFormat Format, VkImageLayout Layout) {
    PushColorAttachment(GetFormatVK(Format), Layout);
}

void VulkanRenderPass::PushColorOutputAttachment() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanSwapChain* swapchain = vulkan->GetSwapChain();

    PushColorAttachment(swapchain->GetChosenSurfaceFormat().format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    ClearExtent = swapchain->GetExtent();
}
void VulkanRenderPass::PushDepthAttachment(TextureFormat Format) {
    VkFormat vk_format = GetFormatVK(Format);
    
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = vk_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //Push new Attachment Description
    mAttachmentDescriptions.push_back(colorAttachment);

    //Create Attachment Reference for Depth Attachment
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment =
        static_cast<unsigned int>(mAttachmentDescriptions.size() - 1);
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //Push new attachment reference
    DepthDescriptionRef = colorAttachmentRef;
    mHasDepthAttachment = true;
}

void VulkanRenderPass::CmdBegin(VulkanCommandBuffer& cmdbuf, VulkanFramebuffer& framebuffer) {
    if (mCreated) {
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = GetRenderPass();
        renderPassInfo.framebuffer = framebuffer.GetFramebuffer();

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = ClearExtent;

        renderPassInfo.clearValueCount = mClearValuesCount;
        renderPassInfo.pClearValues = mClearValues;

        vkCmdBeginRenderPass(cmdbuf.GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
}