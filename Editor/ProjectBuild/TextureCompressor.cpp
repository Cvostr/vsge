#include "TextureCompressor.hpp"
#include <cstring>
#include <bc7enc.h>

using namespace VSGEditor;

void VSGEditor::CompressTexture(VSGE::ProcessedTexture& texture, TargetTextureFormat format)
{
    // Do not convert render targets
    if (texture._target == true)
        return;

	// Convert only RGBA8
	if (texture._format == FORMAT_RGBA && format == BC7)
	{
		// Initialize bc7enc global state
		// This method will take an affect just once
		bc7enc_compress_block_init();

		// Prepare params
		struct bc7enc_compress_block_params bc7params;
		bc7enc_compress_block_params_init(&bc7params);

        // Override format
        texture._format = FORMAT_BC7_UNORM;

		for (int mip_i = 0; mip_i < texture._mip_levels.size(); mip_i++)
		{
			VSGE::TextureMipLevel& tml = texture._mip_levels[mip_i];

			// bc7enc works with RGBA 16x16 blocks
			uint32_t blocks_x = (tml._mip_width + 3) / 4;
			uint32_t blocks_y = (tml._mip_height + 3) / 4;

            byte* srcRgba = tml.GetMipData();
            // alllocate memory for compressed texture
            size_t compressedSize = blocks_x * blocks_y * 16;
			byte *compressed = new byte[compressedSize];

            for (uint32_t by = 0; by < blocks_y; by ++) 
            {
                for (uint32_t bx = 0; bx < blocks_x; bx ++)
                {
                    // Temporary buffer to hold one single uncompressed 4x4 patch (16 pixels * 4 bytes)
                    uint8_t uncompressed_block[64];

                    // Extract the 4x4 pixel data out of the linear row-major image array
                    for (uint32_t y = 0; y < 4; ++y) {
                        uint32_t pixel_y = by * 4 + y;
                        // Clamping handles textures that aren't a perfect multiple of 4
                        if (pixel_y >= tml._mip_height)
                            pixel_y = tml._mip_height - 1;

                        for (uint32_t x = 0; x < 4; ++x) {
                            uint32_t pixel_x = bx * 4 + x;
                            if (pixel_x >= tml._mip_width) 
                                pixel_x = tml._mip_width - 1;

                            // Locate pixel in source image
                            size_t src_idx = (pixel_y * tml._mip_width + pixel_x) * 4;
                            // Locate pixel in the 4x4 block destination buffer
                            size_t dst_idx = (y * 4 + x) * 4;

                            // Copy the RGBA channels (4 bytes)
                            std::memcpy(&uncompressed_block[dst_idx], &srcRgba[src_idx], 4);
                        }
                    }

                    // Target memory where this specific compressed block will be stored
                    uint8_t* pDst_block = &compressed[(by * blocks_x + bx) * 16];

                    // Compress the single 16-pixel block
                    bc7enc_compress_block(pDst_block, uncompressed_block, &bc7params);
                }
            }

            // Free old mip data
            tml.Destroy();
            // Replace mip data with compressed
            tml.SetMipLevelData(compressed, compressedSize);
		}
	}
}