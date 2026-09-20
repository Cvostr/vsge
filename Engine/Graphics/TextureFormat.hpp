#pragma once

enum TextureFormat {
        FORMAT_R = 1,
        FORMAT_RG,
        FORMAT_RGB,
        FORMAT_RGBA,

        FORMAT_R16,
        FORMAT_RG16,
        FORMAT_RGB16,
        FORMAT_RGBA16,

        FORMAT_R16F,
        FORMAT_RG16F,
        FORMAT_RGB16F,
        FORMAT_RGBA16F,

        FORMAT_R32F,
        FORMAT_RG32F,
        FORMAT_RGB32F,
        FORMAT_RGBA32F,

        FORMAT_BC1_UNORM = 0x31545844,      // "DXT1"
        FORMAT_BC2_UNORM = 0x33545844,      // "DXT3"
        FORMAT_BC3_UNORM = 0x35545844,      // "DXT5"

        FORMAT_BC7_UNORM = 0x30315844,      // "DX10"
        FORMAT_BC7_SRGB = 0x30315845,       // "DX10" + 1

        FORMAT_DEPTH_24_STENCIL_8 = 120,
        FORMAT_DEPTH_32,
        FORMAT_DEPTH_32_STENCIL_8
};