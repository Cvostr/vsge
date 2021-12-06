#pragma once

#include <Core/VarTypes/Base.hpp>
#include <vector>
#include <Graphics/TextureFormat.hpp>

namespace VSGE {

	struct TextureMipLevel {
		uint32 _mip_width;
		uint32 _mip_height;

		byte* _mip_data;
		uint32 _mip_data_size;

		TextureMipLevel();

		~TextureMipLevel();

		void Allocate(uint32 size);
		void SetMipLevelData(byte* data, uint32 size);
		void Destroy();
	};

	struct ProcessedTexture {
		TextureFormat _format;

		uint32 _width;
		uint32 _height;

		std::vector<TextureMipLevel> _mip_levels;

		void Destroy();
	};

	void ProcessTexture(byte* data, uint32 size, ProcessedTexture& out);
}