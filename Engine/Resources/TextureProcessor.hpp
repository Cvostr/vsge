#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Graphics/Texture.hpp>
#include <vector>

namespace VSGE {
	struct TextureMipLevel {
		uint32 _mip_width;
		uint32 _mip_height;

		byte* _mip_data;
	};

	struct ProcessedTexture {
		TextureFormat _format;

		uint32 _width;
		uint32 _height;

		std::vector<TextureMipLevel> _mip_levels;
	};

	void ProcessTexture(byte* data, uint32 size, ProcessedTexture& out);
}