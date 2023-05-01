#include "RenderTarget.hpp"

using namespace VSGE;

RenderTarget::RenderTarget() {
	m_width = 0;
	_height = 0;
	_camera_index = 0;
}

uint32 RenderTarget::GetWidth() const {
	return m_width;
}

uint32 RenderTarget::GetHeight() const {
	return _height;
}