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

#define MAP_SIZE 2048

namespace VSGE {

	class VulkanShadowCaster {
	public:
		LightsourceComponent* _lightsource;
		VulkanFramebuffer* _framebuffer;

		VulkanShadowCaster();
		~VulkanShadowCaster();
	};

	class VulkanShadowmapping {
	private:

		VulkanRenderPass* _shadowmap_render_pass;

		VulkanPipeline* _bound_pipeline;

		std::vector<VulkanShadowCaster*> _casters;
		uint32 _added_casters;

		VulkanBuffer* _shadowcasters_buffer;
		//buffer used in shadow rendering stage
		VulkanBuffer* _shadowprocess_buffer;
		VulkanBuffer* _cascadeinfo_buffer;

		VulkanDescriptorPool* _shadowcasters_descPool;
		VulkanDescriptorSet* _shadowcaster_descrSet;

		VulkanDescriptorPool* _shadowrenderer_descPool;
		VulkanDescriptorSet* _shadowrenderer_descrSet;

		std::vector<VulkanDescriptorSet*>* _vertexDescrSets;
		VulkanDescriptorSet* animsDescrSet;

		uint32 _cameraIndex;
		Camera* cam;
		tEntityList* _entitiesToRender;
		VulkanTexture* _gbuffer_pos;

		uint32 _writtenBones;
		uint32 _writtenParticleTransforms;

		uint32 GetShadowTextureIndex(uint32 caster_index, uint32 caster_type);

		void BindPipeline(VulkanCommandBuffer* cmdbuffer, LightType light_type, int mesh_type, uint32 dyn_offset);
	public:

		VulkanShadowmapping(std::vector<VulkanDescriptorSet*>* vertexDescrSets,
			VulkanDescriptorSet* animsDescrSet, 
			VulkanBuffer* cam_buffer,
			VulkanTexture* pos);
		~VulkanShadowmapping();
	
		void SetCamera(uint32 cameraIndex, Camera* cam);
		void SetEntitiesToRender(tEntityList* entities);
		void SetTerrainsToRender(tEntityList* terrains);

		// Register lightcaster entity
		bool AddEntity(Entity* entity);
		void ResetCasters();
		uint32 GetAddedCastersNum() const;
		
		void ProcessShadowCaster(uint32 casterIndex, VulkanCommandBuffer* cmdbuf);
		void ProcessShadowCasters(VulkanCommandBuffer* cmdbuffer);
		
		void UpdateShadowrenderingDescriptors();

		VulkanDescriptorSet* GetShadowRendererDescriptorSet();
	};
}