#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace VSGE;

bool LoadTextureDDS(byte* data, uint32 size, Texture* texture) {
	int maxHeight = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
	int maxWidth = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
	uint32 linearSize = *(reinterpret_cast<uint32*>(&(data[20])));
	uint32 mMipsCount = *(reinterpret_cast<uint32*>(&(data[28])));
	uint32 fourCC = *(reinterpret_cast<uint32*>(&(data[84])));


	uint32 bufsize = mMipsCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size
	byte* bufferT = data + 128; //jumping over header
    //Enum BC
    TextureFormat format = (TextureFormat)fourCC;

	//Getting block size
    uint32 blockSize = (format == FORMAT_BC1_UNORM) ? 8 : 16;
    //Create empty texture
    texture->Create(maxWidth, maxHeight, format, 1, mMipsCount);

    uint32 offset = 0;
    int nwidth = maxWidth;
    int nheight = maxHeight;

    for (uint32 level = 0; level < mMipsCount && (nwidth || nheight); ++level) //Iterating over mipmaps
    {
        size = ((nwidth + 3) / 4) * ((nheight + 3) / 4) * blockSize; //Calculating mip texture size

        texture->AddMipLevel(bufferT + offset, size, nwidth, nheight, level);

        offset += size;
        nwidth /= 2;
        nheight /= 2;
    }

    texture->CreateImageView();

    return true;
}

bool Texture::CreateFromBuffer(byte* data, uint32 size) {
    if (data[0] == 'D' && data[1] == 'D' && data[2] == 'S') {
        return LoadTextureDDS(data, size, this);
    }
    else if (data[1] == 'P' && data[2] == 'N' && data[3] == 'G') {
        int width = 0;
        int height = 0;
        byte* image_data = stbi_load_from_memory(data, size, &width, &height, NULL, 4);
        if (image_data == NULL)
            return false;

        Create(width, height, FORMAT_RGBA, 1, 1);
        AddMipLevel(image_data, 4 * width * height, width, height, 0);
        CreateImageView();

        stbi_image_free(image_data);

        return true;
    }
    else if (data[0] == 'R' && data[1] == 'T' && data[2] == 'B') {
        /*if (engine_ptr->engine_info->graphicsApi == VULKAN) {
            vkTexture* vkt = static_cast<vkTexture*>(this);

            vkt->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            vkt->aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        }*/
        //Create(512, 512, TextureFormat::FORMAT_RGBA);
        //SetRenderTargetFlag(true, true);
    }
    return false;
}