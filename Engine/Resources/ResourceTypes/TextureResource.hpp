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

		~TextureResource() {
			delete _texture;
		}

		Texture* GetTexture() { return _texture; }

		void Release() {
			_texture->Destroy();
		}

		void PostLoad() {
			_texture->CreateFromBuffer(_loadedData, _description.size);
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_TEXTURE;
		}

	};
}