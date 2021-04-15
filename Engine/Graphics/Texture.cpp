#include "Texture.hpp"

using namespace VSGE;

bool Texture::CreateFromBuffer(byte* data, uint32 size) {
    if (data[0] == 'D' && data[1] == 'D' && data[2] == 'S') {
        return CreateFromBufferDDS(data, size);
    }
    else if (data[1] == 'P' && data[2] == 'N' && data[3] == 'G') {
        return CreateFromBufferPNG(data, size);
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