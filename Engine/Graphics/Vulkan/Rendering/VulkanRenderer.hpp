#pragma once

#include "../../Renderer.hpp"
#include "../../Material.hpp"
#include "../VulkanFramebuffer.hpp"
#include "../VulkanRenderpass.hpp"
#include "../VulkanSampler.hpp"
#include "../VulkanSynchronization.hpp"
#include "../VulkanCommandBuffer.hpp"
#include "../VulkanPipeline.hpp"
#include "../Memory/VmaVirtualBlock.hpp"

#include <Scene/EntityComponents/LightProbeComponent.hpp>

#include "VulkanMaterials.hpp"
#include <Graphics/LightsBuffer.hpp>
#include "VulkanTerrainRenderer.hpp"
#include "VulkanUiRenderer.hpp"
#include "VulkanCamerasBuffer.hpp"
#include "PBR/VulkanIrradianceMap.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanFinalPass.hpp"

#include "PBR/VulkanIBL.hpp"
#include <queue>
#include <future>

#define GENERAL_ENV_BUFFER_SIZE		(1024)

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
		VulkanBuffer* _general_env_buffer;
		LightsBuffer* _lights_buffer;

		CVmaVirtualBlock* _materialsVirtBlock;

		VulkanUiRenderer* _ui_renderer;
		VulkanCamerasBuffer* _cameras_buffer;
		VulkanTerrainRenderer* _terrain_renderer;
		std::vector<VulkanRenderTarget*> _render_targets;
		VulkanRenderTarget* _main_render_target;
		VulkanFinalPass* _final_pass;

		// -------------------IBL & probes---------
		VulkanIBL* _ibl_map;
		VulkanLightProbeImage* _generic_probe_image;
		std::queue<LightProbeRenderRequest> probesToRender;
		bool genericProbeUpdateQueued;

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

		VulkanRenderPass* GetMaterialsRenderPass();

		VulkanRenderTarget* GetMainRenderTarget();

		VulkanUiRenderer* GetUiRenderer();
		VulkanFinalPass* GetFinalPass();

		VulkanTexture* GetUiAttachment();

		VulkanCamerasBuffer* GetCamerasBuffer();
		LightsBuffer* GetLightsBuffer();
		VulkanBuffer* GetGeneralEnvBuffer();

		void SetupRenderer();
		void DestroyRenderer();
		void DrawScene(VSGE::Camera* cam);
		void UpdateMaterialDescrSet(Material* mat);
		void EnqueueLightProbe(const LightProbeRenderRequest& component);
		void QueueSkyProbeUpdate();

		static uint32 GetUniformTransformBufferElementSize();

		void ResizeOutput(uint32 width, uint32 height);
	};
}