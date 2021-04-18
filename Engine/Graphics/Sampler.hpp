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
		SAMPLER_WRAP_CLAMP_TO_EDGE
	};

	class Sampler : public IGpuObject {
	protected:
		TextureFilteringMode mMinFiltering;
		TextureFilteringMode mMagFiltering;

		TextureWrapMode mWrapU;
		TextureWrapMode mWrapV;
		TextureWrapMode mWrapW;

		float mMaxAnisotropy;
	public:

		void SetFilteringModes(TextureFilteringMode MinFilter,
			TextureFilteringMode MagFilter)
		{
			mMinFiltering = MinFilter;
			mMagFiltering = MagFilter;
		}

		void SetWrapModes(TextureWrapMode WrapU, TextureWrapMode WrapV, TextureWrapMode WrapW = SAMPLER_WRAP_NONE) {
			mWrapU = WrapU;
			mWrapV = WrapV;

			if (WrapW != SAMPLER_WRAP_NONE)
				mWrapW = WrapW;
		}

		void SetMaxAnisotropy(float Anisotropy) {
			mMaxAnisotropy = Anisotropy;
		}

		virtual bool Create() = 0;

		explicit Sampler() :
			mMinFiltering(SAMPLER_FILTERING_LINEAR),
			mMagFiltering(SAMPLER_FILTERING_LINEAR),

			mWrapU(SAMPLER_WRAP_REPEAT),
			mWrapV(SAMPLER_WRAP_REPEAT),
			mWrapW(SAMPLER_WRAP_REPEAT),

			mMaxAnisotropy(1.f)

		{}
	};
}