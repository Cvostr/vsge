#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanDescriptors.hpp"
#include <Scene/Entity.hpp>

#define MAX_OBJECTS_RENDER 40000
#define MAX_ANIMATION_MATRICES 20000
#define MAX_PARTICLES_MATRICES 20000
#define VERTEX_DESCR_SETS 40

namespace VSGE {
	class VulkanGBufferRenderer {
	private:
		VulkanFramebuffer* _gbuffer_fb;
		VulkanRenderPass* _gbuffer_renderpass;

		//---------------------DESCRIPTORS-----------------
		VulkanDescriptorPool* _objects_descr_pool;
		std::vector<VulkanDescriptorSet*> _vertex_descriptor_sets;
		VulkanDescriptorSet* _animations_descriptor_set;
		VulkanDescriptorSet* _particles_descriptor_set;
		//----------------------BUFFERS----------------------
		VulkanBuffer* _transforms_buffer;
		VulkanBuffer* _animations_buffer;
		VulkanBuffer* _particles_buffer;

		uint32 _fb_width;
		uint32 _fb_height;

		uint32 _camera_index;
		bool _reverseCull;
		tEntityList* _entities_to_render;
		tEntityList* _particles_to_render;

		VulkanPipeline* _boundPipeline;
		void BindPipeline(VulkanCommandBuffer* cmdbuf, VulkanPipeline* pipeline);
	public:

		VulkanGBufferRenderer();
		~VulkanGBufferRenderer();

		void CreateFramebuffer();
		void CreateDescriptorSets();
		void SetCameraIndex(uint32 camera_index);
		void EnableReverseCull();

		void SetEntitiesToRender(tEntityList& entities, tEntityList& particles);
		void SetBuffers(
			VulkanBuffer* transforms_buffer = nullptr,
			VulkanBuffer* anims_buffer = nullptr,
			VulkanBuffer* particles_buffer = nullptr);
		void CreateBuffers();
		void Resize(uint32 width, uint32 height);

		void RecordCmdBuffer(VulkanCommandBuffer* cmdbuf);

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

		std::vector<VulkanDescriptorSet*>& GetVertexDescriptorSets();
		VulkanDescriptorSet* GetAnimationsDescriptorSet();
	};
}