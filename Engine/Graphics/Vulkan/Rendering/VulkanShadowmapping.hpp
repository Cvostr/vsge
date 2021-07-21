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
		VulkanSemaphore* _shadowmapBeginSemaphore;
		VulkanSemaphore* _shadowmapEndSemaphore;

		VulkanRenderPass* _shadowmapRenderPass;

		VulkanCommandPool* _shadowmapCmdPool;
		//-----------------Shadow Pipeline------------
		VertexLayout shadowmap_vertex_layout;
		VulkanShader* _shadowmap_shader;
		VulkanPipelineLayout* _shadowmap_layout;
		VulkanPipeline* _shadowmapPipeline;

		std::vector<VulkanShadowCaster*> _casters;
		uint32 _added_casters;

		VulkanBuffer* _shadowcasters_buffer;
		VulkanDescriptorPool* _shadowcasters_descPool;
		VulkanDescriptorSet* _shadowcaster_descrSet;

		std::vector<VulkanDescriptorSet*>* _vertexDescrSets;
		VulkanDescriptorSet* animsDescrSet;

		Camera* cam;
		std::vector<Entity*>* _entitiesToRender;

		uint32 _writtenBones;
		uint32 _writtenParticleTransforms;
	public:

		VulkanShadowmapping(std::vector<VulkanDescriptorSet*>* vertexDescrSets, VulkanDescriptorSet* animsDescrSet);
		~VulkanShadowmapping();
	
		void SetCamera(Camera* cam) {
			this->cam = cam;
		}
		void SetEntitiesToRender(std::vector<Entity*>* entities) {
			_entitiesToRender = entities;
		}

		void AddEntity(Entity* entity);
		void ResetCasters();
		
		void ProcessShadowCaster(uint32 casterIndex);
		void ExecuteShadowCaster(uint32 casterIndex, VulkanSemaphore* begin = nullptr, VulkanSemaphore* end = nullptr);
	};
}