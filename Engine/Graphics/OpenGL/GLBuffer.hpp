#pragma once

#include "../GpuBuffer.hpp"
#include <GL/glew.h>

namespace VSGE {
	class GLBuffer : public GpuBuffer {
	private:
		GLuint _glBufferId;
	public:
		GLBuffer(GpuBufferType type) {
			mType = type;
		}

		void Create(uint32 size);

		void SetUniformBufferSlot(int slot);

		void Destroy();
	};
}