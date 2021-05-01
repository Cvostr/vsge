#pragma once

#ifdef _WIN32

#include <d3d12.h>
#include <Core/VarTypes/Base.hpp>
#include "../Sampler.hpp"

namespace VSGE {
	class D3D12Sampler : public TextureSampler {
	private:
		D3D12_STATIC_SAMPLER_DESC mSampler;
	public:
		D3D12_STATIC_SAMPLER_DESC GetSampler() { return mSampler; }

		void Destroy();

		D3D12Sampler()
		{}

		~D3D12Sampler() {
			Destroy();
		}
	};
}

#endif