#pragma once

#include "../GpuBuffer.hpp"
#include "VulkanMA.hpp"

namespace VSGE {
	class VulkanBuffer : public GpuBuffer {
	private:
		VmaVkBuffer _buffer;
	public:

		VkBuffer GetBuffer() { return _buffer.Buffer; }

		VulkanBuffer(GpuBufferType type) : GpuBuffer(type) 
		{
		}

		void Create(uint32 size, BufferDeviceLocation location = LOCATION_CPU_GPU);

		void Resize(uint32 new_size);

		void Destroy();

		void WriteData(uint32 offset, uint32 size, void* data);

		~VulkanBuffer() {
			Destroy();
		}
	};
}