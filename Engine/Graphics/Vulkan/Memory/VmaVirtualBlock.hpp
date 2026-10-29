#include "../VulkanBuffer.hpp"
#include <vma/vk_mem_alloc.h>
#include <mutex>

namespace VSGE {
	class CVmaVirtualBlock {
	private:
		VulkanBuffer* _buffer;
		VmaVirtualBlock _vmaVirtualBlock;
		std::mutex _mtx;
	public:
		CVmaVirtualBlock();
		~CVmaVirtualBlock();

		VulkanBuffer* GetBuffer();

		void Create(size_t memsz);
		VmaVirtualAllocation Allocate(size_t size, VkDeviceSize* offset, size_t alignment = 16);
		void Free(VmaVirtualAllocation allocation);
	};
}