#pragma once

#include "GpuObject.hpp"

namespace VSGE {

	enum TextureFilteringMode {
		SAMPLER_FILTERING_NONE,
		SAMPLER_FILTERING_LINEAR,
		SAMPLER_FILTERING_NEAREST
	};

	enum TextureWrapMode {
		SAMPLER_WRAP_NONE,
		SAMPLER_WRAP_REPEAT,
		SAMPLER_WRAP_MIRRORED_REPEAT,
		SAMPLER_WRAP_CLAMP_TO_EDGE,
		SAMPLER_WRAP_CLAMP_TO_BORDER
	};

	enum BorderColor {
		BORDER_COLOR_OPAQUE_WHITE,
		BORDER_COLOR_OPAQUE_BLACK,
		BORDER_COLOR_TRANSPARENT_BLACK
	};

	enum CompareOp {
		COMPARE_OP_NEVER = 0,
		COMPARE_OP_LESS = 1,
		COMPARE_OP_EQUAL = 2,
		COMPARE_OP_LESS_OR_EQUAL = 3,
		COMPARE_OP_GREATER = 4,
		COMPARE_OP_NOT_EQUAL = 5,
		COMPARE_OP_GREATER_OR_EQUAL = 6,
		COMPARE_OP_ALWAYS = 7
	};

	class TextureSampler : public IGpuObject {
	protected:
		TextureFilteringMode _minFiltering;
		TextureFilteringMode _magFiltering;

		TextureWrapMode _wrapU;
		TextureWrapMode _wrapV;
		TextureWrapMode _wrapW;

		BorderColor _borderColor;
		CompareOp _compareOp;

		float _maxAnisotropy;

		float _minLod;
		float _maxLod;
		float _mipLodBias;
	public:

		void SetFilteringModes(TextureFilteringMode MinFilter,
			TextureFilteringMode MagFilter);

		void SetWrapModes(TextureWrapMode WrapU,
						  TextureWrapMode WrapV, 
						  TextureWrapMode WrapW = SAMPLER_WRAP_NONE);

		void SetMaxAnisotropy(float Anisotropy);

		void SetLodsRanges(float minLOD, float maxLOD);

		void SetBorderColor(BorderColor borderColor);

		void SetCompareOp(CompareOp compareOp);

		void SetMipLodBias(float lod_bias);

		virtual bool Create() = 0;

		explicit TextureSampler();
	};
}