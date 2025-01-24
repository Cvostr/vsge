#pragma once

#include <Core/VarTypes/Base.hpp>
#include <vector>
#include <Graphics/TextureFormat.hpp>

namespace VSGE {

	class TextureMipLevel {
	private:
		byte* _mip_data;
		uint32 _mip_data_size;
	public:
		uint32 _mip_width;
		uint32 _mip_height;

		TextureMipLevel();
		~TextureMipLevel();
		TextureMipLevel(const TextureMipLevel&) = delete;
		TextureMipLevel(TextureMipLevel&& level);

		void Allocate(uint32 size);
		void SetMipLevelData(byte* data, uint32 size);
		void Destroy();

		byte* GetMipData() const;
		uint32 GetMipDataSize() const;
	};

	class ProcessedTexture {
	public:
		TextureFormat _format;

		uint32 _width;
		uint32 _height;
		bool _target;

		std::vector<TextureMipLevel> _mip_levels;

		ProcessedTexture() 
		{
			_target = false;
		}

		void Destroy();
		void DumpAsVSTX(byte** out, size_t& out_size);
	};

	void ProcessTexture(byte* data, uint32 size, ProcessedTexture& out);
}