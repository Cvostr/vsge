#pragma once

#include "GpuObject.hpp"
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	enum GpuBufferType {
		GPU_BUFFER_TYPE_NONE,
		GPU_BUFFER_TYPE_VERTEX,
		GPU_BUFFER_TYPE_INDEX,
		GPU_BUFFER_TYPE_UNIFORM,
		GPU_BUFFER_TYPE_STORAGE
	};

	enum BufferDeviceLocation {
		LOCATION_CPU_GPU,
		LOCATION_GPU
	};

	class GpuBuffer : public IGpuObject {
	protected:
		uint32 _size;
		GpuBufferType _type;
		byte* mCpuBuffer;
		BufferDeviceLocation _deviceLocation;
	public:
		GpuBuffer() : 
			_type(GPU_BUFFER_TYPE_NONE),
			_size(0),
			mCpuBuffer(nullptr),
			_deviceLocation(LOCATION_CPU_GPU)
		{}
		GpuBuffer(GpuBufferType type) : 
			_type(type),
			_size(0),
			mCpuBuffer(nullptr),
			_deviceLocation(LOCATION_CPU_GPU)
		{}
		/// <summary>
		/// Get size of buffer
		/// </summary>
		/// <returns></returns>
		uint32 GetSize() { return _size; }
		/// <summary>
		/// Create buffer and allocate memory for it
		/// </summary>
		/// <param name="size">size of buffer to allocate in bytes</param>
		/// <param name="location"></param>
		virtual void Create(uint32 size, BufferDeviceLocation location = LOCATION_CPU_GPU) = 0;
		/// <summary>
		/// Recreate buffer with new size and copy previous data to it
		/// </summary>
		/// <param name="new_size"></param>
		virtual void Resize(uint32 new_size) = 0;
		/// <summary>
		/// Write data to buffer
		/// </summary>
		/// <param name="offset"></param>
		/// <param name="size">- size of data to write</param>
		/// <param name="data">- data to write</param>
		virtual void WriteData(uint32 offset, uint32 size, void* data) = 0;
		/// <summary>
		/// Free buffer
		/// </summary>
		virtual void Destroy() = 0;
	};

	GpuBuffer* CreateGpuBuffer(GpuBufferType type);
}