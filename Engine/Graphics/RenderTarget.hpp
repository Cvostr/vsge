#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class RenderTarget {
	protected:
		uint32 _width;
		uint32 _height;
		uint32 _camera_index;
	public:
		RenderTarget() {
			_width = 0;
			_height = 0;
			_camera_index = 0;
		}
		virtual ~RenderTarget() {

		}
	};
}