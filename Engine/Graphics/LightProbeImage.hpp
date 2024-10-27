#pragma once

#include "Texture.hpp"

namespace VSGE {
	class LightProbeImage : public IGpuObject {

		virtual const Texture* GetSpmap() = 0;
		virtual const Texture* GetIrmap() = 0;

	};
}