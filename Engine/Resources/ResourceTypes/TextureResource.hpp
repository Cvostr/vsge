#pragma once

#include "../Resource.hpp"
#include <Graphics/Texture.hpp>

namespace VSGE {
	class TextureResource : public Resource {
	private:
		Texture* _texture;
		bool _isRenderTarget;
	public:

		TextureResource();

		~TextureResource();

		Texture* GetTexture();

		void Release();

		void PostLoad();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_TEXTURE)
	};
}