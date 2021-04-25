#pragma once

#include <vulkan/vulkan.hpp>
#include "../GpuObject.hpp"
#include <Core/VarTypes/Base.hpp>
#include "VulkanPipelineLayout.hpp"
#include "VulkanDescriptors.hpp"
#include "VulkanBuffer.hpp"

namespace VSGE {

	class VulkanPipeline;

	VkCommandPool beginCommandPool();
	VkCommandBuffer CreateSingleTimeComdbuf(VkCommandPool commandPool);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool);

	class VulkanCommandPool : public IGpuObject {
	private:
		VkCommandPool mCommandPool;
	public:

		VulkanCommandPool() : 
			mCommandPool(VK_NULL_HANDLE)
		{}

		VkCommandPool GetCommandPool() { return mCommandPool; }

		bool Create(uint32 queueFamilyIndex);

		void Destroy();
	};

	class VulkanCommandBuffer : public IGpuObject {
	private:
		VkCommandBuffer mCommandBuffer;
		VulkanCommandPool* mPoolPtr;
	public:

		VulkanCommandBuffer() : 
			mCommandBuffer(VK_NULL_HANDLE),
			mPoolPtr(nullptr)
		{}

		VkCommandBuffer GetCommandBuffer() { return mCommandBuffer; }

		bool Create(VulkanCommandPool& pool);

		void Begin();

		void BindPipeline(VulkanPipeline& pipeline);

		void BindDescriptorSets(VulkanPipelineLayout& layout, uint32 firstSet, uint32 setsCount, VulkanDescriptorSet* sets, uint32 dynOffsetCount = 0, const uint32* offsets = nullptr);

		void BindVertexBuffer(VulkanBuffer& buffer);

		void BindVertexBuffers(VulkanBuffer* buffer, uint32 size = 1, uint32 start = 0);

		void BindIndexBuffer(VulkanBuffer& buffer);

		void Draw(uint32 vertices, uint32 instances = 1, uint32 firstVertex = 0, uint32 firstInstance = 0);

		void DrawIndexed(uint32 indices, uint32 instances = 1, uint32 firstIndex = 0, uint32 firstInstance = 0);

		void SetViewports(uint32 firstViewport, uint32 count, VkViewport* viewports);

		void SetViewport(float x, float y, float width, float height, float depthMin = 0.f, float depthMax = 1.f, uint32 index = 0);

		void EndRenderPass();

		void End();

		void Destroy();
	};
}