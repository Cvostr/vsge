#pragma once

#include "../VulkanFramebuffer.hpp"
#include "../VulkanCommandBuffer.hpp"
#include "../VulkanSynchronization.hpp"
#include <Scene/EntityComponents/LightComponent.hpp>
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

#define SHADOWPROCESS_BUFFER_SIZE 44036
#define SHADOWPROCESS_SHADOWCOUNT_OFFSET 44032

namespace VSGE {

	class VulkanShadowCaster {
	public:
		LightsourceComponent* _lightsource;
		VulkanFramebuffer* _framebuffer;

		VulkanShadowCaster() {
			_framebuffer = new VulkanFramebuffer;
			_framebuffer->SetLayersCount(0);
			_lightsource = nullptr;
		}

		~VulkanShadowCaster() {
			delete _framebuffer;
		}
	};

	class VulkanShadowmapping {
	private:

		VulkanRenderPass* _shadowmapRenderPass;
		VulkanRenderPass* _shadowmap_point_RenderPass;
		VulkanRenderPass* _shadowprocessRenderPass;

		//-----------------Shadow Pipeline------------
		VertexLayout shadowmap_vertex_layout;
		VulkanShader* _shadowmap_shader;
		VulkanShader* _shadowmap_point_shader;
		VulkanPipelineLayout* _shadowmap_layout;
		VulkanPipeline* _shadowmapPipeline;
		VulkanPipeline* _shadowmap_point_Pipeline;
		//-----------------Shadowprocess pipeline
		VertexLayout _shadowprocess_vertex_layout;
		VulkanShader* _shadowprocess_shader;
		VulkanPipelineLayout* _shadowprocess_layout;
		VulkanPipeline* _shadowprocess_pipeline;
		VulkanFramebuffer* _shadowprocess_framebuffer;

		std::vector<VulkanShadowCaster*> _casters;
		uint32 _added_casters;

		VulkanBuffer* _shadowcasters_buffer;
		//buffer used in shadow rendering stage
		VulkanBuffer* _shadowprocess_buffer;
		VulkanBuffer* _cascadeinfo_buffer;

		VulkanDescriptorPool* _shadowcasters_descPool;
		VulkanDescriptorSet* _shadowcaster_descrSet;
		VulkanDescriptorSet* _shadowrenderer_descrSet;

		std::vector<VulkanDescriptorSet*>* _vertexDescrSets;
		VulkanDescriptorSet* animsDescrSet;

		Camera* cam;
		tEntityList* _entitiesToRender;
		tEntityList* _terrainsToRender;
		VulkanTexture* _gbuffer_pos;
		VulkanSampler* _shadowmap_sampler;

		uint32 _writtenBones;
		uint32 _writtenParticleTransforms;

		uint32 _outputWidth;
		uint32 _outputHeight;

		uint32 GetShadowTextureIndex(uint32 caster_index, uint32 caster_type);
	public:

		VulkanShadowmapping(std::vector<VulkanDescriptorSet*>* vertexDescrSets,
			VulkanDescriptorSet* animsDescrSet, 
			VulkanBuffer* cam_buffer,
			VulkanTexture* pos);
		~VulkanShadowmapping();
	
		void SetCamera(Camera* cam);
		void SetEntitiesToRender(tEntityList* entities);
		void SetTerrainsToRender(tEntityList* terrains);
		void SetGbufferPositionsAttachment(VulkanTexture* gpos);

		void ResizeOutput(uint32 width, uint32 height);
		VulkanTexture* GetOutputTexture();

		void AddEntity(Entity* entity);
		void ResetCasters();
		
		void ProcessShadowCaster(uint32 casterIndex, VulkanCommandBuffer* cmdbuf);
		void ProcessShadowCasters(VulkanCommandBuffer* cmdbuffer);
		
		void UpdateShadowrenderingDescriptors();
		void RecordShadowProcessingCmdbuf(VulkanCommandBuffer* cmdbuf);
	};
}