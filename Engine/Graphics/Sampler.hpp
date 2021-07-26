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

	class TextureSampler : public IGpuObject {
	protected:
		TextureFilteringMode _minFiltering;
		TextureFilteringMode _magFiltering;

		TextureWrapMode _wrapU;
		TextureWrapMode _wrapV;
		TextureWrapMode _wrapW;

		BorderColor _borderColor;

		float _maxAnisotropy;
	public:

		void SetFilteringModes(TextureFilteringMode MinFilter,
			TextureFilteringMode MagFilter)
		{
			_minFiltering = MinFilter;
			_magFiltering = MagFilter;
		}

		void SetWrapModes(TextureWrapMode WrapU, TextureWrapMode WrapV, TextureWrapMode WrapW = SAMPLER_WRAP_NONE) {
			_wrapU = WrapU;
			_wrapV = WrapV;

			if (WrapW != SAMPLER_WRAP_NONE)
				_wrapW = WrapW;
		}

		void SetMaxAnisotropy(float Anisotropy) {
			_maxAnisotropy = Anisotropy;
		}

		void SetBorderColor(BorderColor borderColor) {
			_borderColor = borderColor;
		}

		virtual bool Create() = 0;

		explicit TextureSampler() :
			_minFiltering(SAMPLER_FILTERING_LINEAR),
			_magFiltering(SAMPLER_FILTERING_LINEAR),

			_wrapU(SAMPLER_WRAP_REPEAT),
			_wrapV(SAMPLER_WRAP_REPEAT),
			_wrapW(SAMPLER_WRAP_REPEAT),

			_borderColor(BORDER_COLOR_OPAQUE_BLACK),

			_maxAnisotropy(1.f)

		{}
	};
}