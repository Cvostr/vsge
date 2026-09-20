#pragma once

#include <Resources/TextureProcessor.hpp>

namespace VSGEditor 
{
	enum TargetTextureFormat {
		BC7,
		ASTC
	};

	void CompressTexture(VSGE::ProcessedTexture &texture, TargetTextureFormat format);
}