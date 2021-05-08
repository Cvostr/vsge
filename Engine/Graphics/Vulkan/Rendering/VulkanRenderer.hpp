#pragma once

#include "../../Renderer.hpp"

#include "../VulkanFramebuffer.hpp"
#include "../VulkanRenderpass.hpp"
#include "../VulkanDescriptors.hpp"
#include "../VulkanSampler.hpp"
#include "../VulkanSynchronization.hpp"
#include "../VulkanCommandBuffer.hpp"

#define MAX_OBJECTS_RENDER 10000
#define MAX_ANIMATION_MATRICES 10000

namespace VSGE {

	class VulkanDrawCall {
	public:
		Entity* entity;
		Mat4 transformMatrix;
		VulkanDescriptorSet* sets[4];
	};

	class VulkanRenderer : public IRenderer {
	private:

		static VulkanRenderer* _this;

		VulkanFramebuffer* mGBuffer;
		VulkanRenderPass* mGBufferPass;

		VulkanFramebuffer* mOutputBuffer;
		VulkanRenderPass* mOutputPass;
		//--------------------Semaphores-----------------
		VulkanSemaphore* mBeginSemaphore;
		VulkanSemaphore* mGBufferSemaphore;
		VulkanSemaphore* mEndSemaphore;
		//--------------------Command buffers------------
		VulkanCommandPool* mCmdPool;
		VulkanCommandBuffer* mGBufferCmdbuf;
		VulkanCommandBuffer* mLightsCmdbuf;
		
		//--------------------Descriptors----------------
		VulkanDescriptorPool* mObjectsPool;
		std::vector<VulkanDescriptorSet*> mVertexDescriptorSets;

		//--------------------Buffers--------------------
		VulkanBuffer* mCameraShaderBuffer;
		VulkanBuffer* mTransformsShaderBuffer;
		VulkanBuffer* mAnimationTransformsShaderBuffer;

		std::vector<VulkanDrawCall> drawCalls;
	public:

		VulkanRenderer() : IRenderer() {
			SetupRenderer();
			_this = this;
		}

		~VulkanRenderer() {
			DestroyRenderer();
			_this = this;
		}

		static VulkanRenderer* Get() {
			return _this;
		}

		VulkanSemaphore* GetBeginSemaphore() {
			return mBeginSemaphore;
		}

		VulkanSemaphore* GetEndSemaphore() {
			return mEndSemaphore;
		}

		void StoreWorldObjects();

		void SetupRenderer();
		void DestroyRenderer();
		void DrawScene();
	};
}