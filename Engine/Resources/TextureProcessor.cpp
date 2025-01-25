#include "TextureProcessor.hpp"
#include <Graphics/Texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#include "Core/ByteSerialize.hpp"
#include "Core/ByteSolver.hpp"

using namespace VSGE;

TextureMipLevel::TextureMipLevel() 
{
    _mip_data = nullptr;
    _mip_data_size = 0;

    _mip_width = 0;
    _mip_height = 0;
}

TextureMipLevel::~TextureMipLevel() 
{
    SAFE_RELEASE_ARR(_mip_data)
}

TextureMipLevel::TextureMipLevel(TextureMipLevel&& level)
{
    _mip_data = level._mip_data;
    _mip_data_size = level._mip_data_size;
    _mip_width = level._mip_width;
    _mip_height = level._mip_height;

    level._mip_data = nullptr;
    level._mip_data_size = 0;
}

byte* TextureMipLevel::GetMipData() const
{
    return _mip_data;
}

uint32 TextureMipLevel::GetMipDataSize() const
{
    return _mip_data_size;
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
void TextureMipLevel::Destroy() 
{
    SAFE_RELEASE_ARR(_mip_data);
}

ProcessedTexture::ProcessedTexture()
{
    _format = FORMAT_RGBA;
    _target = false;
    _width = 0;
    _height = 0;
}

ProcessedTexture::~ProcessedTexture()
{
    Destroy();
}

void ProcessedTexture::Destroy() 
{
    for (auto& mip : _mip_levels) {
        mip.Destroy();
    }
    _mip_levels.clear();
}

void ProcessedTexture::DumpAsVSTX(byte** out, size_t& out_size)
{
    ByteSerialize* serializer = new ByteSerialize;

    serializer->WriteBytes("VSTX", 4);
    serializer->Serialize(_width);
    serializer->Serialize(_height);
    serializer->Serialize(_format);
    serializer->Serialize(1);
    serializer->Serialize((uint32)_mip_levels.size());
    serializer->Serialize(_target); // non target

    // Write mips to dictionary
    size_t offset_acc = 0;
    for (size_t mip_i = 0; mip_i < _mip_levels.size(); mip_i++)
    {
        const TextureMipLevel& mip = _mip_levels[mip_i];
        serializer->Serialize(mip._mip_width);
        serializer->Serialize(mip._mip_height);
        serializer->Serialize(mip.GetMipDataSize());
        serializer->Serialize(offset_acc);

        offset_acc += mip.GetMipDataSize();
    }

    // Write mips data contigiously
    for (size_t mip_i = 0; mip_i < _mip_levels.size(); mip_i++)
    {
        const TextureMipLevel& mip = _mip_levels[mip_i];
        serializer->WriteBytes(mip.GetMipData(), mip.GetMipDataSize());
    }

    out_size = serializer->GetSerializedSize();
    *out = new byte[out_size];
    memcpy(*out, serializer->GetBytes(), out_size);
}

void VSGE::ProcessTexture(byte* data, uint32 size, ProcessedTexture& out) {
    if (data == nullptr || size == 0)
        return;

    bool is_png = data[1] == 'P' && data[2] == 'N' && data[3] == 'G';
    bool is_jpg = data[0] == 0xFF && data[1] == 0xD8;
    bool is_dds = data[0] == 'D' && data[1] == 'D' && data[2] == 'S';
    bool is_vstx = data[0] == 'V' && data[1] == 'S' && data[2] == 'T' && data[3] == 'X';

    if (is_vstx) 
    {
        ByteSolver vstx_solver(data, size);
        vstx_solver.move(4); // skip VSTX header
        out._width = vstx_solver.GetValue<uint32>();
        out._height = vstx_solver.GetValue<uint32>();
        out._format = vstx_solver.GetValue<TextureFormat>();
        uint32 layers = vstx_solver.GetValue<uint32>();
        uint32 mips = vstx_solver.GetValue<uint32>();
        out._target = vstx_solver.GetValue<bool>();

        if (out._target == true)
        {
            // This texture is render target for cameras
            TextureMipLevel main_level;
            main_level._mip_width = 512;
            main_level._mip_height = 512;
            out._mip_levels.push_back(std::move(main_level));
        }
        else {
            size_t start_offset = vstx_solver.offset() + mips * (4 + 4 + 4 + 8);
            // read dictionary
            for (size_t mip_i = 0; mip_i < mips; mip_i++)
            {
                TextureMipLevel mip_level;
                mip_level._mip_width = vstx_solver.GetValue<uint32>();
                mip_level._mip_height = vstx_solver.GetValue<uint32>();
                uint32 mip_data_size = vstx_solver.GetValue<uint32>();
                size_t offset = vstx_solver.GetValue<size_t>();

                mip_level.SetMipLevelData(data + start_offset + offset, mip_data_size);
                out._mip_levels.push_back(std::move(mip_level));
            }
        }
    }
    else if (is_png || is_jpg) {
        int width = 0;
        int height = 0;
        int channels = 0;

        // Unpack to RGBA8
        byte* image_data = stbi_load_from_memory(data, size, &width, &height, &channels, STBI_rgb_alpha);
    
        out._format = TextureFormat::FORMAT_RGBA;
        out._width = width;
        out._height = height;

        TextureMipLevel main_level;
        main_level._mip_width = width;
        main_level._mip_height = height;
        main_level.SetMipLevelData(image_data, width * height * 4);
        out._mip_levels.push_back(std::move(main_level));

        int cur_width = width;
        int cur_height = height;
        int minw = 8;
        int minh = 8;

        while (cur_width > minw && cur_height > minh)
        {
            cur_width /= 2;
            cur_height /= 2;

            // alloc buffer for downsized image
            size_t buffer_size = static_cast<size_t>(cur_width * cur_height * 4);
            byte* temp = new byte[buffer_size];

            // resize image to a half of sizes
            stbir_resize_uint8(image_data, width, height, 0, temp, cur_width, cur_height, 0, 4);

            // add as mip map
            TextureMipLevel level;
            level._mip_width = cur_width;
            level._mip_height = cur_height;
            level.SetMipLevelData(temp, buffer_size);
            out._mip_levels.push_back(std::move(level));

            delete[] temp;
        }

        // free texture memory 
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
            out._mip_levels.push_back(std::move(mip_level));

            offset += size;
            nwidth /= 2;
            nheight /= 2;
        }
    }
}