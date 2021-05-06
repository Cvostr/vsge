#include "VulkanRenderer.hpp"
#include "../VulkanRAPI.hpp"

using namespace VSGE;

void VulkanRenderer::SetupRenderer() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	mGBufferPass = new VulkanRenderPass;
	mGBufferPass->PushColorAttachment(FORMAT_RGBA);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA16F);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA16F);
	mGBufferPass->PushDepthAttachment();
	mGBufferPass->Create();

	mGBuffer = new VulkanFramebuffer;
	mGBuffer->SetSize(mOutputWidth, mOutputHeight);
	mGBuffer->AddAttachment(FORMAT_RGBA); //Color
	mGBuffer->AddAttachment(FORMAT_RGBA16F); //Normal
	mGBuffer->AddAttachment(FORMAT_RGBA16F); //Position
	mGBuffer->AddDepth();
	mGBuffer->Create(mGBufferPass);

	mOutputPass = new VulkanRenderPass;
	mOutputPass->PushColorAttachment(FORMAT_RGBA);
	mOutputPass->Create();

	mOutputBuffer = new VulkanFramebuffer;
	mOutputBuffer->SetSize(mOutputWidth, mOutputHeight);
	mOutputBuffer->AddAttachment(FORMAT_RGBA);
	mOutputBuffer->Create(mOutputPass);
	
	mOutput = mOutputBuffer->GetColorAttachments()[0];
	
	//-----------------------Prepare semaphores-------------------
	mImageReadySemaphore = new VulkanSemaphore;
	mImageReadySemaphore->Create();

	mGBufferSemaphore = new VulkanSemaphore;
	mGBufferSemaphore->Create();

	mLightSemaphore = new VulkanSemaphore;
	mLightSemaphore->Create();

	mObjectsPool = new VulkanDescriptorPool;


	//---------------------Command buffers
	mCmdPool = new VulkanCommandPool;
	mCmdPool->Create(device->GetGraphicsQueueFamilyIndex());

	mGBufferCmdbuf = new VulkanCommandBuffer;
	mGBufferCmdbuf->Create(mCmdPool);
}

void VulkanRenderer::DestroyRenderer() {

}

void VulkanRenderer::DrawScene() {

}