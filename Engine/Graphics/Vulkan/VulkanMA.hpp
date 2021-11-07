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

	class VulkanCommandPool;
	class VulkanCommandBuffer;

	struct ThreadCmdbufPair {
		uint32 thread_id;

		uint32 family_index;
		VulkanCommandBuffer* cmdbuf;
		VkQueue transfer_queue;
	};

	class VulkanMA {
	private:
		void* allocator;

		std::vector<ThreadCmdbufPair*> _threaded_cmdbufs;
		std::vector<VulkanCommandPool*> _family_cmdpools;

	public:

		VulkanMA(VulkanInstance* inst, VulkanDevice* device);
		void allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer);
		void allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, uint32 size);
		void allocateCpu(VkBufferUsageFlags flags, VmaVkBuffer* buffer, uint32 size, void** mapped);
		void allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, void* data, uint32 size);
		void copy(VkBuffer buffer, uint32 offset, void* data, uint32 size);
		bool createImage(VkImageCreateInfo* info, VmaVkImage* image);

		ThreadCmdbufPair* GetTransferCmdbufThreaded();

		void map(VmaVkBuffer* buf, void** mem);
		void unmap(VmaVkBuffer* buf);
		void destroyBuffer(VmaVkBuffer* buf);
		void destroyImage(VmaVkImage* image);
	};
}