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
	case GPU_BUFFER_TYPE_STORAGE:
		return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	return 0;
}


void VulkanBuffer::Create(uint32 size, BufferDeviceLocation location) {
	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanMA* vulkan_ma = vulkan_rapi->GetAllocator();

	_deviceLocation = location;
	_size = size;

	if (location == LOCATION_CPU_GPU) {
		vulkan_ma->allocateCpu(GetBufferTypeVK(_type), &mBuffer, size, (void**)&this->mCpuBuffer);
		memset(mCpuBuffer, 0, size);
		vulkan_ma->unmap(&mBuffer);
	}
	if (location == LOCATION_GPU)
		vulkan_ma->allocate(GetBufferTypeVK(_type), &mBuffer, nullptr, size);
	mCreated = true;
}

void VulkanBuffer::Resize(uint32 new_size) {
	//VulkanBuffer* temp_buffer = new VulkanBuffer(GPU_)
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
	if (size == 0)
		//if nothing to copy, then exit
		return;

	VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
	VulkanMA* vulkan_ma = vulkan_rapi->GetAllocator();
	if (_deviceLocation == LOCATION_CPU_GPU) {
		vulkan_ma->map(&mBuffer, (void**)&mCpuBuffer);
		memcpy(this->mCpuBuffer + offset, data, size);
		vulkan_ma->unmap(&mBuffer);
	}
	else if (_deviceLocation == LOCATION_GPU) {
		vulkan_ma->copy(mBuffer.Buffer, offset, data, size);
	}
}