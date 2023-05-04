#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class RenderTarget {
	protected:
		uint32 m_width;
		uint32 m_height;
		uint32 _camera_index;
	public:
		RenderTarget();

		virtual ~RenderTarget() {

		}

		uint32 GetWidth() const;

		uint32 GetHeight() const;
	};
}