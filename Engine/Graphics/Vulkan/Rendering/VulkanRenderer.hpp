#pragma once

#include "../../Renderer.hpp"
#include "../../Material.hpp"
#include "../VulkanFramebuffer.hpp"
#include "../VulkanRenderpass.hpp"
#include "../VulkanSampler.hpp"
#include "../VulkanSynchronization.hpp"
#include "../VulkanCommandBuffer.hpp"
#include "../VulkanPipeline.hpp"

#include "VulkanMaterials.hpp"
#include "VulkanShadowmapping.hpp"

#define MAX_OBJECTS_RENDER 40000
#define MAX_ANIMATION_MATRICES 20000
#define MAX_PARTICLES_MATRICES 20000

#define MAX_CAMERAS 10
#define UNI_ALIGN 256
#define MATERIAL_SIZE 512

#define VERTEX_DESCR_SETS 40

namespace VSGE {

	class VulkanRenderer : public IRenderer {
	private:

		static VulkanRenderer* _this;

		VulkanFramebuffer* mGBuffer;
		VulkanRenderPass* mGBufferPass;

		VulkanFramebuffer* mOutputBuffer;
		VulkanRenderPass* mOutputPass;
		
		//--------------------Semaphores-----------------
		VulkanSemaphore* mBeginSemaphore;
		VulkanSemaphore* mShadowmappingEndSemaphore;
		VulkanSemaphore* mShadowprocessingEndSemaphore;
		VulkanSemaphore* mGBufferSemaphore;
		VulkanSemaphore* mEndSemaphore;

		//--------------------Command buffers------------
		VulkanCommandPool* mCmdPool;
		VulkanCommandBuffer* mGBufferCmdbuf;
		VulkanCommandBuffer* mLightsCmdbuf;
		//--------------------Descriptors----------------
		VulkanDescriptorPool* mObjectsPool;
		VulkanDescriptorPool* mMaterialsDescriptorPool;
		std::vector<VulkanDescriptorSet*> mVertexDescriptorSets;
		VulkanDescriptorSet* mAnimationsDescriptorSet;
		VulkanDescriptorSet* mParticlesDescriptorSet;

		VulkanDescriptorSet* mDeferredPassSet;
		//--------------------Buffers--------------------
		VulkanBuffer* mCameraShaderBuffer;
		VulkanBuffer* mTransformsShaderBuffer;
		VulkanBuffer* mAnimationTransformsShaderBuffer;
		VulkanBuffer* mParticlesTransformShaderBuffer;
		VulkanBuffer* _lightsBuffer;
		//--------------------Meshes--------------------
		VulkanMesh* mSpriteMesh;

		//--------------------Samplers-------------------
		VulkanSampler* mMaterialMapsSampler;
		VulkanSampler* mAttachmentSampler;
		//--------------------Empty texture----------
		VulkanTexture* mEmptyTexture;

		//--------------------Pipelines------------------
		VulkanPipeline* mDeferredPipeline;

		//--------------------Shadowmapping--------------
		VulkanShadowmapping* _shadowmapper;


		MaterialTemplate* pbr_template;
		MaterialTemplate* particle_template;

		VulkanPipeline* CreatePipelineFromMaterialTemplate(MaterialTemplate* mat_template);

		VulkanMaterial* CreateVulkanMaterial(Material* material);

		VulkanDescriptorSet* CreateDescriptorSetFromMaterialTemplate(MaterialTemplate* mat_template);

		void BindMaterial(Material* mat);
	
		uint32 _writtenBones;
		uint32 _writtenParticleTransforms;

		//TEMPORARY 
		Camera* cam;
		//-----------------------

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

		VulkanSampler* GetAttachmentSampler() {
			return mAttachmentSampler;
		}

		void StoreWorldObjects();

		void SetupRenderer();
		void DestroyRenderer();
		void DrawScene(VSGE::Camera* cam);
	};
}