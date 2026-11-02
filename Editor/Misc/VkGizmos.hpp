#pragma once

#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include "Graphics/Vulkan/Rendering/VulkanCamerasBuffer.hpp"
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class VkGizmos {
	private:
		static VkGizmos* _this;

		VSGE::VulkanShader* _object_shader;

		VSGE::VulkanPipeline* _object_pipeline;
		VSGE::VulkanPipelineLayout* _object_pipeline_layout;

		uint32 _fb_width;
		uint32 _fb_height;

		VSGE::VulkanFramebuffer* _framebuffer;
		VSGE::VulkanRenderPass* _rp;

		VSGE::VulkanCamerasBuffer* _cameras_buffer;

		VSGE::VulkanBuffer* _transforms_buffer;
		VSGE::VulkanBuffer* _animations_buffer;

		VSGE::VulkanDescriptorPool* _objects_descr_pool;
		VSGE::VulkanDescriptorSet* _transforms_descriptor_set;
		VSGE::VulkanDescriptorSet* _animations_descriptor_set;

		std::vector<VSGE::Entity*> _picked_entities;

		bool _draw_gizmos;
	public:

		static VkGizmos* Get() {
			return _this;
		}

		VkGizmos();
		~VkGizmos();

		void Create();
		void Destroy();

		void AddPickedEntity(VSGE::Entity* entity);
		void RemovePickedEntity(VSGE::Entity* entity);
		void ClearPickedEntities();

		void UpdateCamera(VSGE::Camera *cam);
		void PrepareBuffers();
		void AppendToCmdbuf(VSGE::VulkanCommandBuffer* cmdbuf);

		VSGE::VulkanTexture* GetOutputTexture();
	};
}