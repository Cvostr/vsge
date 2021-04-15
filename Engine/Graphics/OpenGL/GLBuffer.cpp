#include "GLBuffer.hpp"

using namespace VSGE;

GLenum GetBufferTypeGL(GpuBufferType type) {
	switch (type) {
	case GPU_BUFFER_TYPE_INDEX:
		return GL_ELEMENT_ARRAY_BUFFER;
	case GPU_BUFFER_TYPE_UNIFORM:
		return GL_UNIFORM_BUFFER;
	case GPU_BUFFER_TYPE_STORAGE:
		return GL_SHADER_STORAGE_BUFFER;
	}
	return GL_NONE;
}

void GLBuffer::Create(uint32 size) {
	glGenBuffers(1, &_glBufferId);
	GLenum gltype = GetBufferTypeGL(mType);
	glBindBuffer(gltype, _glBufferId); //Bind vertex buffer
	glBufferData(gltype, size, nullptr, GL_STATIC_DRAW); //send vertices to buffer
}

void GLBuffer::SetUniformBufferSlot(int slot) {
	GLenum gltype = GetBufferTypeGL(mType);
	glBindBufferBase(gltype, slot, _glBufferId);
}

void GLBuffer::Destroy() {
	glDeleteBuffers(1, &_glBufferId);
}