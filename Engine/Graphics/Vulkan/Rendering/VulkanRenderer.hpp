#pragma once

#include "../../Renderer.hpp"

#include "../VulkanFramebuffer.hpp"
#include "../VulkanRenderpass.hpp"
#include "../VulkanDescriptors.hpp"
#include "../VulkanSampler.hpp"
#include "../VulkanSynchronization.hpp"
#include "../VulkanCommandBuffer.hpp"

namespace VSGE {

	class VulkanDrawCall {
	public:
		Entity* entity;
		Mat4 transformMatrix;
		std::vector<VulkanDescriptorSet*> sets;
	};

	class VulkanRenderer : public IRenderer {
	private:

		VulkanFramebuffer* mGBuffer;
		VulkanRenderPass* mGBufferPass;

		VulkanFramebuffer* mOutputBuffer;
		VulkanRenderPass* mOutputPass;
		//--------------------Semaphores-----------------
		VulkanSemaphore* mImageReadySemaphore;
		VulkanSemaphore* mGBufferSemaphore;
		VulkanSemaphore* mLightSemaphore;
		//--------------------Command buffers------------
		VulkanCommandPool* mCmdPool;
		VulkanCommandBuffer* mGBufferCmdbuf;
		
		VulkanDescriptorPool* mObjectsPool;
	public:

		VulkanRenderer() : IRenderer() {
			SetupRenderer();
		}

		~VulkanRenderer() {
			DestroyRenderer();
		}

		void SetupRenderer();
		void DestroyRenderer();
		void DrawScene();
	};
}