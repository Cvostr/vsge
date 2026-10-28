#include "../VulkanBuffer.hpp"
#include <vma/vk_mem_alloc.h>

namespace VSGE {
	class CVmaVirtualBlock {
	private:
		VulkanBuffer* _buffer;
		VmaVirtualBlock _vmaVirtualBlock;
	public:
		CVmaVirtualBlock();
		~CVmaVirtualBlock();

		VulkanBuffer* GetBuffer();

		void Create(size_t memsz);
		VmaVirtualAllocation Allocate(size_t size, size_t alignment = 16);
		void Free(VmaVirtualAllocation allocation);
	};
}