#pragma once

#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Scene/Entity.hpp>

namespace VSGEditor {
	class VkGizmos {
	private:
		static VkGizmos* _this;

		VSGE::VulkanShader* _object_shader;

		VSGE::VulkanPipeline* _object_pipeline;
		VSGE::VulkanPipelineLayout* _object_pipeline_layout;

		std::vector<VSGE::Entity*> _picked_entities;
		std::vector<VSGE::Entity*>* _entities_to_draw;

		bool _draw_gizmos;
	public:

		static VkGizmos* Get() {
			return _this;
		}

		VkGizmos();
		~VkGizmos();

		void Create();
		void Destroy();

		void SetEntitiesToDraw(std::vector<VSGE::Entity*>& entities);
		void AddPickedEntity(VSGE::Entity* entity);
		void ClearPickedEntities();

		void AppendToCmdbuf(VSGE::VulkanCommandBuffer* cmdbuf);
	};
}