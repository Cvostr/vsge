//#define VMA_IMPLEMENTATION
#include "VmaVirtualBlock.hpp"

using namespace VSGE;

CVmaVirtualBlock::CVmaVirtualBlock() :
	_vmaVirtualBlock(nullptr)
{
	_buffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
}

CVmaVirtualBlock::~CVmaVirtualBlock()
{
	delete _buffer;
}

VulkanBuffer* CVmaVirtualBlock::GetBuffer()
{
	return _buffer;
}

void CVmaVirtualBlock::Create(size_t memsz)
{
	VmaVirtualBlockCreateInfo blockCreateInfo = {};
	blockCreateInfo.size = memsz;
	blockCreateInfo.flags = 0;
	blockCreateInfo.pAllocationCallbacks = nullptr;

	VkResult result = vmaCreateVirtualBlock(&blockCreateInfo, &_vmaVirtualBlock);

	if (result != VK_SUCCESS) {
		// TODO: handle
		throw std::runtime_error("Error creating Vulkan Memory Allocator VirtualBlock");
	}

	// Create real buffer
	_buffer->Create(memsz);
}

VmaVirtualAllocation CVmaVirtualBlock::Allocate(size_t size, VkDeviceSize* offset, size_t alignment)
{
	VmaVirtualAllocation resultAlloc;

	VmaVirtualAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.size = size;				// Size of this data in bytes
	allocCreateInfo.alignment = alignment;		// Align for GPU hardware efficiency
	allocCreateInfo.flags = 0;					// Standard allocation allocation strategy
	allocCreateInfo.pUserData = nullptr;		// Optional: debug label

	VkResult result = vmaVirtualAllocate(
		_vmaVirtualBlock,   // The block to allocate inside of
		&allocCreateInfo,   // The allocation requirements
		&resultAlloc,		// Output: The unique handle for this allocation
		offset				// Output: The calculated byte offset within the block
	);

	return result == VK_SUCCESS ? resultAlloc : VK_NULL_HANDLE;
}

void CVmaVirtualBlock::Free(VmaVirtualAllocation allocation)
{
	vmaVirtualFree(_vmaVirtualBlock, allocation);
}