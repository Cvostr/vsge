#pragma once

#include "../GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "VulkanRenderpass.hpp"
#include "VulkanShader.hpp"
#include "VulkanPipelineLayout.hpp"
#include "../VertexLayout.hpp"

namespace VSGE {

	struct VulkanPipelineConf {
		VkPrimitiveTopology primitiveTopology;
		VkPolygonMode polygonMode;
		VkCullModeFlagBits cullFaceMode;
		VkFrontFace frontFace;
		bool DepthTest;
		VkCompareOp DepthOp;
		VkViewport Viewport;
		
		VulkanPipelineConf() {
			primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			polygonMode = VK_POLYGON_MODE_FILL;
			cullFaceMode = VK_CULL_MODE_NONE;
			DepthTest = false;
			DepthOp = VK_COMPARE_OP_LESS;

			Viewport.x = 0.0f;
			Viewport.y = 0.0f;
			Viewport.width = 0.f;
			Viewport.height = 0.f;
			Viewport.minDepth = 0.0f;
			Viewport.maxDepth = 1.0f;
		}

	};


	class VulkanPipeline : public IGpuObject {
	private:
		VkPipeline mPipeline;

		VulkanPipelineConf conf;
		VertexLayout vert_layout;
	public:

		VkPipeline GetPipeline() { return mPipeline; }

		bool Create(VulkanPipelineConf& Conf, VulkanShader& shader, VulkanRenderPass& rpass, VertexLayout& vl, VulkanPipelineLayout& layout);

		void Destroy();

	};
}