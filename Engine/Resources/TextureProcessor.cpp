#include "TextureProcessor.hpp"
#include <Graphics/Texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace VSGE;

TextureMipLevel::TextureMipLevel() {
    _mip_data = nullptr;
}

TextureMipLevel::~TextureMipLevel() {

}

void TextureMipLevel::Allocate(uint32 size) {
    Destroy();
    _mip_data = new byte[size];
}
void TextureMipLevel::SetMipLevelData(byte* data, uint32 size) {
    _mip_data_size = size;
    Allocate(size);
    memcpy(_mip_data, data, size);
}
void TextureMipLevel::Destroy() {
    SAFE_RELEASE_ARR(_mip_data);
}

void ProcessedTexture::Destroy() {
    for (auto& mip : _mip_levels) {
        mip.Destroy();
    }
    _mip_levels.clear();
}

void VSGE::ProcessTexture(byte* data, uint32 size, ProcessedTexture& out) {
    if (data == nullptr || size == 0)
        return;

    bool is_png = data[1] == 'P' && data[2] == 'N' && data[3] == 'G';
    bool is_jpg = data[0] == 0xFF && data[1] == 0xD8;
    bool is_dds = data[0] == 'D' && data[1] == 'D' && data[2] == 'S';

    if (is_png || is_jpg) {
        int width = 0;
        int height = 0;

        byte* image_data = stbi_load_from_memory(data, size, &width, &height, NULL, 4);
    
        out._format = TextureFormat::FORMAT_RGBA;
        out._width = width;
        out._height = height;

        TextureMipLevel main_level;
        main_level._mip_width = width;
        main_level._mip_height = height;
        main_level.SetMipLevelData(image_data, width * height * 4);
        out._mip_levels.push_back(main_level);

        stbi_image_free(image_data);
    }
    else if (is_dds) {
        out._height = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
        out._width = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
        uint32 fourCC = *(reinterpret_cast<uint32*>(&(data[84])));
        out._format = (TextureFormat)fourCC;
        uint32 mipsCount = *(reinterpret_cast<uint32*>(&(data[28])));
        uint32 linearSize = *(reinterpret_cast<uint32*>(&(data[20])));

        uint32 bufsize = mipsCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size
        byte* bufferT = data + 128; //jumping over header
        //Getting block size
        uint32 blockSize = (out._format == FORMAT_BC1_UNORM) ? 8 : 16;

        uint32 offset = 0;
        int nwidth = out._width;
        int nheight = out._height;

        for (uint32 level = 0; level < mipsCount; ++level) //Iterating over mipmaps
        {
            size = ((nwidth + 3) / 4) * ((nheight + 3) / 4) * blockSize; //Calculating mip texture size

            TextureMipLevel mip_level;
            mip_level._mip_width = nwidth;
            mip_level._mip_height = nheight;
            mip_level.SetMipLevelData(bufferT + offset, size);
            out._mip_levels.push_back(mip_level);

            offset += size;
            nwidth /= 2;
            nheight /= 2;
        }
    }
}