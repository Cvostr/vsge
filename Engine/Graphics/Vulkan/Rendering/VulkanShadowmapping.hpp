#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanCommandBuffer.hpp"
#include "../VulkanSynchronization.hpp"
#include <vector>
#include <Scene/Entity.hpp>
#include <Graphics/VertexLayout.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include "../VulkanBuffer.hpp"
#include "../VulkanDescriptors.hpp"

#define UNI_ALIGN 256
#define MAP_SIZE 2048

#define SHADOWMAP_BUFFER_ELEMENT_SIZE 768
#define SHADOWPROCESS_BUFFER_ELEMENT_SIZE 688

namespace VSGE {

	class VulkanShadowCaster {
	public:
		VulkanFramebuffer* _framebuffer;
		VulkanCommandBuffer* _cmdbuf;

		VulkanShadowCaster() {
			_framebuffer = new VulkanFramebuffer;
			_framebuffer->SetLayersCount(0);
			_cmdbuf = new VulkanCommandBuffer;
		}

		~VulkanShadowCaster() {
			delete _framebuffer;
			delete _cmdbuf;
		}
	};

	class VulkanShadowmapping {
	private:
		//--------------Semaphores------------
		VulkanSemaphore* _shadowmapFirstSemaphore;
		VulkanSemaphore* _shadowmapSecondSemaphore;

		VulkanRenderPass* _shadowmapRenderPass;
		VulkanRenderPass* _shadowprocessRenderPass;

		VulkanCommandPool* _shadowmapCmdPool;
		//-----------------Shadow Pipeline------------
		VertexLayout shadowmap_vertex_layout;
		VulkanShader* _shadowmap_shader;
		VulkanPipelineLayout* _shadowmap_layout;
		VulkanPipeline* _shadowmapPipeline;
		//-----------------Shadowprocess pipeline
		VertexLayout _shadowprocess_vertex_layout;
		VulkanShader* _shadowprocess_shader;
		VulkanPipelineLayout* _shadowprocess_layout;
		VulkanPipeline* _shadowprocess_pipeline;
		VulkanFramebuffer* _shadowprocess_framebuffer;
		VulkanCommandBuffer* _shadowprocess_cmdbuf;

		std::vector<VulkanShadowCaster*> _casters;
		uint32 _added_casters;

		VulkanBuffer* _shadowcasters_buffer;
		VulkanBuffer* _shadowprocess_buffer;

		VulkanDescriptorPool* _shadowcasters_descPool;
		VulkanDescriptorSet* _shadowcaster_descrSet;
		VulkanDescriptorSet* _shadowrenderer_descrSet;

		std::vector<VulkanDescriptorSet*>* _vertexDescrSets;
		VulkanDescriptorSet* animsDescrSet;

		Camera* cam;
		std::vector<Entity*>* _entitiesToRender;
		VulkanTexture* _gbuffer_pos;
		VulkanSampler* _gbuffer_sampler;
		VulkanMesh* _screenPlane;
		VulkanSampler* _shadowmap_sampler;

		uint32 _writtenBones;
		uint32 _writtenParticleTransforms;
	public:

		VulkanShadowmapping(std::vector<VulkanDescriptorSet*>* vertexDescrSets,
			VulkanDescriptorSet* animsDescrSet, 
			VulkanBuffer* cam_buffer,
			VulkanMesh* screenPlane,
			VulkanTexture* pos, 
			VulkanSampler* gbuffer_sampler);
		~VulkanShadowmapping();
	
		void SetCamera(Camera* cam) {
			this->cam = cam;
		}
		void SetEntitiesToRender(std::vector<Entity*>* entities) {
			_entitiesToRender = entities;
		}

		void ResizeOutput(uint32 width, uint32 height);
		VulkanTexture* GetOutputTexture();

		void AddEntity(Entity* entity);
		void ResetCasters();
		
		void ProcessShadowCaster(uint32 casterIndex);
		void ExecuteShadowCaster(uint32 casterIndex, VulkanSemaphore* begin = nullptr, VulkanSemaphore* end = nullptr);
		
		void RecordShadowProcessingCmdbuf();
		void RenderShadows(VulkanSemaphore* begin, VulkanSemaphore* end);
	};
}