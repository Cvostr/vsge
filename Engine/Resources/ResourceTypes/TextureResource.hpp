#pragma once

#include "../Resource.hpp"
#include <Graphics/Texture.hpp>

namespace VSGE {
	class TextureResource : public Resource {
	private:
		Texture* _texture;
	public:

		TextureResource() {
			_texture = CreateTexture();
		}

		Texture* GetTexture() { return _texture; }

		void PostLoad(byte* data, uint32 size) {
			_texture->CreateFromBuffer(data, size);
		}

	};
}