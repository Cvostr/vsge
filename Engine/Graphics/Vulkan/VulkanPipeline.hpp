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
		VkBool32 primitiveRestartEnable;

		VkPolygonMode polygonMode;
		VkCullModeFlagBits cullFaceMode;
		VkFrontFace frontFace;
		VkBool32 DepthTest;
		VkBool32 StencilTest;
		VkCompareOp DepthOp;
		
		VulkanPipelineConf() {
			primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			primitiveRestartEnable = VK_FALSE;

			frontFace = VK_FRONT_FACE_CLOCKWISE;
			polygonMode = VK_POLYGON_MODE_FILL;
			cullFaceMode = VK_CULL_MODE_NONE;
			DepthTest = false;
			StencilTest = false;
			DepthOp = VK_COMPARE_OP_LESS;
		}

	};


	class VulkanPipeline : public IGpuObject {
	private:
		VkPipeline mPipeline;
		VulkanPipelineLayout* _pipelineLayout;

		VulkanPipelineConf conf;
		VertexLayout vert_layout;
	public:

		VulkanPipeline() : 
			mPipeline(VK_NULL_HANDLE)
		{}

		VkPipeline GetPipeline() { return mPipeline; }

		VulkanPipelineLayout* GetPipelineLayout() {
			return _pipelineLayout;
		}

		bool Create(VulkanPipelineConf& Conf, VulkanShader* shader, VulkanRenderPass* rpass, VertexLayout& vl, VulkanPipelineLayout* layout);

		void Destroy();

		~VulkanPipeline() {
			Destroy();
		}
	};
}