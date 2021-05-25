#include "VulkanBuffer.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkBufferUsageFlags GetBufferTypeVK(GpuBufferType type) {
	switch (type) {
	case GPU_BUFFER_TYPE_VERTEX:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	case GPU_BUFFER_TYPE_INDEX:
		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	case GPU_BUFFER_TYPE_UNIFORM:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	case GPU_BUFFER_TYPE_DYNBUFFER:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	case GPU_BUFFER_TYPE_STORAGE:
		return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	return 0;
}


void VulkanBuffer::Create(uint32 size, BufferDeviceLocation location) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanMA* vulkan_ma = vulkan_rapi->GetAllocator();

	mDeviceLocation = location;
	mSize = size;

	if (location == LOCATION_CPU_GPU) {
		vulkan_ma->allocateCpu(GetBufferTypeVK(mType), &mBuffer, size, (void**)&this->mCpuBuffer);
		memset(mCpuBuffer, 0, size);
		vulkan_ma->unmap(&mBuffer);
	}
	if (location == LOCATION_GPU)
		vulkan_ma->allocate(GetBufferTypeVK(mType), &mBuffer, nullptr, size);
	mCreated = true;
}

void VulkanBuffer::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
		VulkanMA* vulkan_ma = vulkan_rapi->GetAllocator();

		vulkan_ma->destroyBuffer(&mBuffer);
		mCreated = false;
	}
}

void VulkanBuffer::WriteData(uint32 offset, uint32 size, void* data) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanMA* vulkan_ma = vulkan_rapi->GetAllocator();
	if (mDeviceLocation == LOCATION_CPU_GPU) {
		vulkan_ma->map(&mBuffer, (void**)&mCpuBuffer);
		memcpy(this->mCpuBuffer + offset, data, size);
		vulkan_ma->unmap(&mBuffer);
	}
	else if (mDeviceLocation == LOCATION_GPU) {
		vulkan_ma->copy(mBuffer.Buffer, offset, data, size);
	}
}