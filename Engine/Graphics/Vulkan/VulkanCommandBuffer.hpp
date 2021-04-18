#pragma once

#include <vulkan/vulkan.hpp>
#include "../GpuObject.hpp"
#include <Core/VarTypes/Base.hpp>

namespace VSGE {
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
	public:

		VulkanCommandBuffer() : 
			mCommandBuffer(VK_NULL_HANDLE)
		{}

		VkCommandBuffer GetCommandBuffer() { return mCommandBuffer; }

		bool Create(VulkanCommandPool& pool);

		void Begin();
		void End();

		void Destroy();
	};
}