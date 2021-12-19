#pragma once

#include "Texture.hpp"
#include <Math/Vec2.hpp>

namespace VSGE {
	class PostprocessEffect {
	protected:
		Texture* _input;

		Vec2i _output_size;
	public:
		virtual void SetInputTexture(Texture* input) {
			_input = input;
		}

		virtual void ResizeOutput(const Vec2i& new_size) = 0;
	};
}