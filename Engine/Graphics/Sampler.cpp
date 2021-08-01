#include "Sampler.hpp"

using namespace VSGE;

void TextureSampler::SetFilteringModes(TextureFilteringMode MinFilter,
	TextureFilteringMode MagFilter)
{
	_minFiltering = MinFilter;
	_magFiltering = MagFilter;
}

void TextureSampler::SetWrapModes(
	TextureWrapMode WrapU, 
	TextureWrapMode WrapV, 
	TextureWrapMode WrapW
) {
	_wrapU = WrapU;
	_wrapV = WrapV;

	if (WrapW != SAMPLER_WRAP_NONE)
		_wrapW = WrapW;
}

void TextureSampler::SetMaxAnisotropy(float Anisotropy) {
	_maxAnisotropy = Anisotropy;
}

void TextureSampler::SetLodsRanges(float minLOD, float maxLOD) {
	_minLod = minLOD;
	_maxLod = maxLOD;
}

void TextureSampler::SetMipLodBias(float lod_bias) {
	_mipLodBias = lod_bias;
}

void TextureSampler::SetBorderColor(BorderColor borderColor) {
	_borderColor = borderColor;
}

void TextureSampler::SetCompareOp(CompareOp compareOp) {
	_compareOp = compareOp;
}

TextureSampler::TextureSampler() :
	_minFiltering(SAMPLER_FILTERING_LINEAR),
	_magFiltering(SAMPLER_FILTERING_LINEAR),

	_wrapU(SAMPLER_WRAP_REPEAT),
	_wrapV(SAMPLER_WRAP_REPEAT),
	_wrapW(SAMPLER_WRAP_REPEAT),

	_borderColor(BORDER_COLOR_OPAQUE_BLACK),
	_compareOp(COMPARE_OP_NEVER),

	_maxAnisotropy(1.f),

	_minLod(0.f),
	_maxLod(1.f),
	_mipLodBias(0.f)

{}