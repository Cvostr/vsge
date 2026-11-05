#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanDescriptors.hpp"
#include <Scene/Entity.hpp>
#include "VulkanCamerasBuffer.hpp"

#define MAX_OBJECTS_RENDER 40000
#define MAX_ANIMATION_MATRICES 20000
#define MAX_PARTICLES_MATRICES 20000

namespace VSGE {

	enum WorldRenderMode {
		RENDER_NONE,
		RENDER_STATIC_ONLY,
		RENDER_ALL
	};

	class VulkanGBufferRenderer {
	private:
		VulkanFramebuffer* m_gbufferFramebuffer;
		VulkanRenderPass* _gbuffer_renderpass;

		//---------------------DESCRIPTORS-----------------
		VulkanDescriptorPool* _objects_descr_pool;
		VulkanDescriptorSet* _transforms_descriptor_set;
		VulkanDescriptorSet* _animations_descriptor_set;
		VulkanDescriptorSet* _particles_descriptor_set;
		//----------------------BUFFERS----------------------
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _animations_buffer;
		VulkanBuffer* _particles_buffer;
		VulkanCamerasBuffer* camerasBuffer;

		uint32 _fb_width;
		uint32 _fb_height;

		WorldRenderMode renderMode;
		uint32 _camera_index;
		bool _reverseCull;
		tEntityList* _entities_to_render;
		tEntityList* _particles_to_render;
		Scene* _scene;

		VulkanPipeline* _boundPipeline;
		void BindPipeline(VulkanCommandBuffer* cmdbuf, VulkanPipeline* pipeline);
	public:

		VulkanGBufferRenderer();
		~VulkanGBufferRenderer();

		void CreateFramebuffer();
		void CreateDescriptorSets(VulkanCamerasBuffer* camerasBuffer, VulkanBuffer* generalEnvBuffer);
		void SetCameraIndex(uint32 camera_index);
		void EnableReverseCull();

		void SetEntitiesToRender(tEntityList& entities, tEntityList& particles);
		tEntityList* GetEntitiesToRender();
		tEntityList* GetParticlesToRender();

		void SetRenderMode(WorldRenderMode mode);
		void SetScene(Scene* scene);
		void SetBuffers(
			VulkanBuffer* transforms_buffer = nullptr,
			VulkanBuffer* anims_buffer = nullptr,
			VulkanBuffer* particles_buffer = nullptr);
		void CreateBuffers();
		void Resize(uint32 width, uint32 height);

		void RecordCmdBuffer(VulkanCommandBuffer* cmdbuf);

		Scene* GetScene();
		VulkanFramebuffer* GetFramebuffer();
		VulkanRenderPass* GetRenderPass();
		VulkanTexture* GetAlbedoAttachment();
		VulkanTexture* GetNormalAttachment();
		VulkanTexture* GetPositionAttachment();
		VulkanTexture* GetMaterialsAttachment();
		VulkanTexture* GetDepthAttachment();
		VulkanBuffer* GetTransformsBuffer();
		VulkanBuffer* GetAnimationsBuffer();
		VulkanBuffer* GetParticlesBuffer();

		VulkanDescriptorSet* GetVertexDescriptorSet();
		VulkanDescriptorSet* GetParticlesDescriptorSet();
		VulkanDescriptorSet* GetAnimationsDescriptorSet();
	};
}