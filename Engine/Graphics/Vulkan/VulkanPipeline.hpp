#pragma once

#include "../GraphicsPipeline.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanRenderpass.hpp"
#include "VulkanShader.hpp"
#include "VulkanPipelineLayout.hpp"
#include "../VertexLayout.hpp"

namespace VSGE {

	class VulkanPipeline : public GraphicsPipeline {
	private:
		VkPipeline _pipeline;
		VulkanPipelineLayout* _pipelineLayout;
		bool _dynamic_cull_mode;
	public:

		VulkanPipeline();

		~VulkanPipeline();

		VkPipeline GetPipeline();

		VulkanPipelineLayout* GetPipelineLayout();

		void SetDynamicCullMode(bool dynamic_cull_mode);

		bool Create(VulkanShader* shader, VulkanRenderPass* rpass, VertexLayout& vl, VulkanPipelineLayout* layout);

		void Destroy();
	};
}