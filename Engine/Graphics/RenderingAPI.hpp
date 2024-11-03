#pragma once

#include <Engine/Window.hpp>

namespace VSGE {
	class IRenderingAPI {
	public:
		IRenderingAPI(){}

		virtual void Create(Window* window, bool debugGraphics) = 0;
		virtual void Destroy() = 0;
	};
}