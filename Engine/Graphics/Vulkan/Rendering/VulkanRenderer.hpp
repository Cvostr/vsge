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
#include <Graphics/LightsBuffer.hpp>
#include "VulkanTerrainRenderer.hpp"
#include "VulkanUiRenderer.hpp"
#include "VulkanCamerasBuffer.hpp"
#include "PBR/VulkanIrradianceMap.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanFinalPass.hpp"

#include "PBR/VulkanIBL.hpp"

#define UNI_ALIGN 256
#define MATERIAL_SIZE 512

namespace VSGE {

	class VulkanRenderer : public IRenderer {
	private:

		static VulkanRenderer* _this;
		
		//--------------------Semaphores-----------------
		VulkanSemaphore* mBeginSemaphore;
		VulkanSemaphore* mEndSemaphore;

		//--------------------Command buffers------------
		VulkanCommandPool* _cmdpool;
		VulkanCommandBuffer* _render_targets_cmdbuf;
		//--------------------Descriptors----------------
		VulkanDescriptorPool* mMaterialsDescriptorPool;

		//--------------------Buffers--------------------
		VulkanBuffer* mTransformsShaderBuffer;
		VulkanBuffer* mAnimationTransformsShaderBuffer;
		VulkanBuffer* mParticlesTransformShaderBuffer;
		LightsBuffer* _lights_buffer;
		//--------------------Meshes--------------------
		VulkanMesh* mSpriteMesh;

		//--------------------Samplers-------------------
		VulkanSampler* mAttachmentSampler;

		VulkanShadowmapping* _shadowmapper;
		VulkanTerrainRenderer* _terrain_renderer;
		VulkanUiRenderer* _ui_renderer;
		Vulkan_BRDF_LUT* _brdf_lut;
		VulkanCamerasBuffer* _cameras_buffer;
		VulkanIBL* _ibl_map;
		std::vector<VulkanRenderTarget*> _render_targets;
		VulkanRenderTarget* _main_render_target;
		VulkanFinalPass* _final_pass;

		VulkanPipeline* CreatePipelineFromMaterialTemplate(MaterialTemplate* mat_template);

		VulkanMaterial* CreateVulkanMaterial(Material* material);

		VulkanDescriptorSet* CreateDescriptorSetFromMaterialTemplate(MaterialTemplate* mat_template);
	
		uint32 _writtenBones;
		uint32 _writtenParticleTransforms;

		void StoreWorldObjects(Camera* cam);

		void FillCommandBuffers();
	public:

		VulkanRenderer();

		~VulkanRenderer();

		static VulkanRenderer* Get() {
			return _this;
		}

		VulkanCommandPool* GetCommandPool();

		VulkanSemaphore* GetBeginSemaphore();
		void SetBeginSemaphore(VulkanSemaphore* semaphore);

		VulkanSemaphore* GetEndSemaphore();

		VulkanSampler* GetAttachmentSampler();

		VulkanMesh* GetScreenMesh();

		VulkanCamerasBuffer* GetCamerasBuffer();

		Vulkan_BRDF_LUT* GetBRDF();

		VulkanRenderPass* GetMaterialsRenderPass();

		VulkanRenderTarget* GetMainRenderTarget();

		VulkanUiRenderer* GetUiRenderer();

		VulkanFinalPass* GetFinalPass();

		VulkanTexture* GetUiAttachment();

		LightsBuffer* GetLightsBuffer();

		void SetupRenderer();
		void DestroyRenderer();
		void DrawScene(VSGE::Camera* cam);
		void UpdateMaterialDescrSet(Material* mat);

		VulkanTerrainRenderer* GetTerrainRenderer();

		void ResizeOutput(uint32 width, uint32 height);
	};
}