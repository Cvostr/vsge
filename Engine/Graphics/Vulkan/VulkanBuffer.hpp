#pragma once

#include "../GpuBuffer.hpp"
#include "VulkanMA.hpp"

#define UNI_ALIGN (uint64)VSGE::VulkanRAPI::Get()->GetDevice()->GetUniformBufferMinAlignment()

namespace VSGE {
	class VulkanBuffer : public GpuBuffer {
	private:
		VmaVkBuffer _buffer;
	public:

		VkBuffer GetBuffer();

		VulkanBuffer(GpuBufferType type);
		VulkanBuffer(const VulkanBuffer&) = delete;
		~VulkanBuffer();

		void Create(uint32 size, BufferDeviceLocation location = LOCATION_CPU_GPU);

		void Resize(uint32 new_size);

		void Destroy();

		void WriteData(uint32 offset, uint32 size, void* data);
	};
}