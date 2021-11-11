#pragma once

#include "../Resource.hpp"
#include <Graphics/Texture.hpp>
#include "../TextureProcessor.hpp"

namespace VSGE {
	class TextureResource : public Resource {
	private:
		Texture* _texture;

		ProcessedTexture _processed_texture;
	public:

		TextureResource();

		~TextureResource();

		Texture* GetTexture();

		void OnRelease();

		void PostLoad();

		void Prepare();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_TEXTURE)
	};
}