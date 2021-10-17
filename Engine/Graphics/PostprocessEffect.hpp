#pragma once

#include "Texture.hpp"

namespace VSGE {
	class PostprocessEffect {
	protected:
		Texture* _input;
		Texture* _output;
	public:
		Texture* GetOutputTexture() {
			return _output;
		}
		virtual void SetInputTexture(Texture* input) {
			_input = input;
		}
	};
}