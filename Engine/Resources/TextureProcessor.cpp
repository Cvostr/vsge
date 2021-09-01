#include "TextureProcessor.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace VSGE;

void VSGE::ProcessTexture(byte* data, uint32 size, ProcessedTexture& out) {
    if (data == nullptr || size == 0)
        return;

    bool is_png = data[1] == 'P' && data[2] == 'N' && data[3] == 'G';
    bool is_jpg = data[0] == 0xFF && data[1] == 0xD8;
    bool is_dds = data[0] == 'D' && data[1] == 'D' && data[2] == 'S';
}