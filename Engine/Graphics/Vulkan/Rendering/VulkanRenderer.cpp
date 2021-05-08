#include "VulkanRenderer.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanShader.hpp"

using namespace VSGE;

VulkanRenderer* VulkanRenderer::_this = nullptr;

void VulkanRenderer::SetupRenderer() {

	ShaderStorage::Get()->LoadShaderBundle("../Shaders/shaders.bundle", "../Shaders/shaders.map");
	VulkanShader* pbr = new VulkanShader;
	pbr->AddShaderFromFile("pbr.vert", SHADER_STAGE_VERTEX);
	pbr->AddShaderFromFile("pbr.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(pbr, "PBR");

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	mGBufferPass = new VulkanRenderPass;
	mGBufferPass->SetClearSize(mOutputWidth, mOutputHeight);
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
	mOutputPass->SetClearSize(mOutputWidth, mOutputHeight);
	mOutputPass->PushColorAttachment(FORMAT_RGBA);
	mOutputPass->Create();

	mOutputBuffer = new VulkanFramebuffer;
	mOutputBuffer->SetSize(mOutputWidth, mOutputHeight);
	mOutputBuffer->AddAttachment(FORMAT_RGBA);
	mOutputBuffer->Create(mOutputPass);
	
	mOutput = mOutputBuffer->GetColorAttachments()[0];
	
	//-----------------------Prepare semaphores-------------------
	mBeginSemaphore = new VulkanSemaphore;
	mBeginSemaphore->Create();

	mGBufferSemaphore = new VulkanSemaphore;
	mGBufferSemaphore->Create();

	mEndSemaphore = new VulkanSemaphore;
	mEndSemaphore->Create();

	//---------------------Buffers--------------------------------
	mCameraShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mCameraShaderBuffer->Create(32);

	mTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mTransformsShaderBuffer->Create(sizeof(Mat4) * MAX_OBJECTS_RENDER);

	mAnimationTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mAnimationTransformsShaderBuffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	//----------------------Descriptors--------------------------
	mObjectsPool = new VulkanDescriptorPool;

	for (uint32 desc_i = 0; desc_i < MAX_OBJECTS_RENDER; desc_i++) {
		VulkanDescriptorSet* set = new VulkanDescriptorSet(mObjectsPool);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_VERTEX_BIT); //Camera
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT); //Transform
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, VK_SHADER_STAGE_VERTEX_BIT); //Animation

		mVertexDescriptorSets.push_back(set);
	}
	//Create POOL
	mObjectsPool->Create();
	//Create descriptor sets
	for (uint32 desc_i = 0; desc_i < MAX_OBJECTS_RENDER; desc_i++) {
		VulkanDescriptorSet* set = mVertexDescriptorSets[desc_i];
		set->Create();

		set->WriteDescriptorBuffer(0, mCameraShaderBuffer);
		set->WriteDescriptorBuffer(1, mTransformsShaderBuffer, 0, 64);
		set->WriteDescriptorBuffer(2, mAnimationTransformsShaderBuffer, 0, 64);
	}

	//---------------------Command buffers------------------------
	mCmdPool = new VulkanCommandPool;
	mCmdPool->Create(device->GetGraphicsQueueFamilyIndex());

	mGBufferCmdbuf = new VulkanCommandBuffer;
	mGBufferCmdbuf->Create(mCmdPool);

	mLightsCmdbuf = new VulkanCommandBuffer;
	mLightsCmdbuf->Create(mCmdPool);

	
}

void VulkanRenderer::DestroyRenderer() {

}

void VulkanRenderer::StoreWorldObjects() {
	CreateRenderList();
	Mat4* transforms = new Mat4[mEntitiesToRender.size()];
	for (uint32 e_i = 0; e_i < mEntitiesToRender.size(); e_i ++) {
		Entity* entity = mEntitiesToRender[e_i];
		transforms[e_i] = entity->GetWorldTransform();
	}
	mGBufferCmdbuf->Begin();
	mGBufferPass->CmdBegin(*mGBufferCmdbuf, *mGBuffer);

	for (uint32 e_i = 0; e_i < mEntitiesToRender.size(); e_i++) {
		
	}

	mGBufferCmdbuf->EndRenderPass();
	mGBufferCmdbuf->End();

}

void VulkanRenderer::DrawScene() {
	StoreWorldObjects();

	VulkanGraphicsSubmit(*mGBufferCmdbuf, *mBeginSemaphore, *mEndSemaphore);
}