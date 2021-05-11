#pragma once

#include "../../Renderer.hpp"
#include "../../Material.hpp"
#include "../VulkanFramebuffer.hpp"
#include "../VulkanRenderpass.hpp"
#include "../VulkanDescriptors.hpp"
#include "../VulkanSampler.hpp"
#include "../VulkanSynchronization.hpp"
#include "../VulkanCommandBuffer.hpp"
#include "../VulkanPipeline.hpp"

#define MAX_OBJECTS_RENDER 10000
#define MAX_ANIMATION_MATRICES 10000
#define UNI_ALIGN 256
#define MATERIAL_SIZE 512

namespace VSGE {

	class VulkanDrawCall {
	public:
		Entity* entity;
		Mat4 transformMatrix;
		VulkanPipeline* pipeline;
		VulkanDescriptorSet* sets[4];
	};

	typedef std::pair<Guid, VulkanPipeline*> GuidedVulkanPipeline;

	class MaterialPipelines {
		std::vector<GuidedVulkanPipeline> _pipelines;

		VulkanPipeline* GetPipeline(const Guid& guid);
		void Add(const Guid& guid, VulkanPipeline* pipeline);
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
		VulkanBuffer* mMaterialsShaderBuffer;

		//--------------------Samplers-------------------
		VulkanSampler* mMaterialMapsSampler;

		std::vector<VulkanDrawCall> drawCalls;

		MaterialTemplate* pbr_template;
		Material* pbr_material;

		VulkanPipeline* test;

		VulkanPipeline* CreatePipelineFromMaterialTemplate(MaterialTemplate* mat_template);
		void BindMaterial(Material* mat);

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
		void DrawScene(VSGE::Camera* cam);
	};
}