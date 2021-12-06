#pragma once

#include "Texture.hpp"
#include <Math/Vec2.hpp>

namespace VSGE {
	class PostprocessEffect {
	protected:
		Texture* _input;
		Texture* _output;

		Vec2i _output_size;
	public:
		Texture* GetOutputTexture() {
			return _output;
		}
		virtual void SetInputTexture(Texture* input) {
			_input = input;
		}
	};
}