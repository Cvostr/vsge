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
#include "VulkanTerrainRenderer.hpp"
#include "VulkanDeferredLight.hpp"
#include "VulkanCamerasBuffer.hpp"
#include "VulkanGBufferRenderer.hpp"
#include "VulkanEnvMap.hpp"

#define UNI_ALIGN 256
#define MATERIAL_SIZE 512

namespace VSGE {

	class VulkanRenderer : public IRenderer {
	private:

		static VulkanRenderer* _this;
		
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
		VulkanDescriptorPool* mMaterialsDescriptorPool;

		//--------------------Buffers--------------------
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
		VulkanTexture* mEmptyZeroTexture;
		VulkanTexture* mEmptyOneTexture;


		VulkanShadowmapping* _shadowmapper;
		VulkanTerrainRenderer* _terrain_renderer;
		Vulkan_BRDF_LUT* _brdf_lut;
		VulkanCamerasBuffer* _cameras_buffer;
		VulkanGBufferRenderer* _gbuffer_renderer;
		VulkanDeferredLight* _deferred_renderer;
		VulkanEnvMap* _env_map;

		MaterialTemplate* pbr_template;
		MaterialTemplate* particle_template;
		MaterialTemplate* skybox_template;

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
			_this = this;
			SetupRenderer();
		}

		~VulkanRenderer() {
			DestroyRenderer();
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

		VulkanMesh* GetScreenMesh() {
			return mSpriteMesh;
		}

		VulkanCamerasBuffer* GetCamerasBuffer() {
			return _cameras_buffer;
		}

		void StoreWorldObjects();

		void SetupRenderer();
		void DestroyRenderer();
		void DrawScene(VSGE::Camera* cam);
		void UpdateMaterialDescrSet(Material* mat);

		VulkanTerrainRenderer* GetTerrainRenderer();

		void ResizeOutput(uint32 width, uint32 height);
	};
}