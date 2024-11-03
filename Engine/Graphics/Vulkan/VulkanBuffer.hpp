#pragma once

#include "../GpuBuffer.hpp"
#include "VulkanMA.hpp"

#define UNI_ALIGN (uint64)VSGE::VulkanRAPI::Get()->GetDevice()->GetUniformBufferMinAlignment()

namespace VSGE {
	class VulkanBuffer : public GpuBuffer {
	private:
		VmaVkBuffer _buffer;
	public:

		VkBuffer GetBuffer() { return _buffer.Buffer; }

		VulkanBuffer(GpuBufferType type) : GpuBuffer(type) 
		{
		}

		VulkanBuffer(const VulkanBuffer&) = delete;

		void Create(uint32 size, BufferDeviceLocation location = LOCATION_CPU_GPU);

		void Resize(uint32 new_size);

		void Destroy();

		void WriteData(uint32 offset, uint32 size, void* data);

		~VulkanBuffer() {
			Destroy();
		}
	};
}