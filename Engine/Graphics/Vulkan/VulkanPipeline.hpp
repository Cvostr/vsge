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
	public:

		VulkanPipeline() : 
			_pipeline(VK_NULL_HANDLE),
			_pipelineLayout(nullptr)
		{}

		VkPipeline GetPipeline() { return _pipeline; }

		VulkanPipelineLayout* GetPipelineLayout() {
			return _pipelineLayout;
		}

		bool Create(VulkanShader* shader, VulkanRenderPass* rpass, VertexLayout& vl, VulkanPipelineLayout* layout);

		void Destroy();

		~VulkanPipeline() {
			Destroy();
		}
	};
}