#pragma once

#include "VulkanDevice.hpp"

namespace VSGE {
	struct VmaVkBuffer {
		VkBuffer Buffer;
		void* _allocation;

		VmaVkBuffer() :
			Buffer(VK_NULL_HANDLE),
			_allocation(nullptr)
		{
		}
	};

	struct VmaVkImage {
		VkImage Image;
		void* _allocation;
		VmaVkImage() :
			Image(VK_NULL_HANDLE),
			_allocation(nullptr)
		{
			
		}
	};

	class VulkanMA {
	private:
		void* allocator;

		VkCommandPool MemoryCommandPool;
		VkCommandBuffer MemoryCommandBuffer;

	public:

		VulkanMA(VulkanInstance* inst, VulkanDevice* device);
		void allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer);
		void allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, unsigned int size);
		void allocateCpu(VkBufferUsageFlags flags, VmaVkBuffer* buffer, unsigned int size, void** mapped);
		void allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, void* data, unsigned int size);
		void copy(VkBuffer buffer, unsigned int offset, void* data, unsigned int size);

		VkCommandBuffer GetSingleTimeCmdBuf() { return MemoryCommandBuffer; }
		VkCommandPool GetSingleTimeCmdPool() { return MemoryCommandPool; }
		bool createImage(VkImageCreateInfo* info, VmaVkImage* image);

		void map(VmaVkBuffer* buf, void** mem);
		void unmap(VmaVkBuffer* buf);
		void destroyBuffer(VmaVkBuffer* buf);
		void destroyImage(VmaVkImage* image);
	};
}