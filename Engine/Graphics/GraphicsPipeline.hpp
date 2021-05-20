#pragma once

#include "GpuObject.hpp"

namespace VSGE {

	enum FrontFaceMode {
		FRONT_FACE_CCW = 0,
		FRONT_FACE_MODE_CW = 1
	};

	enum CullMode {
		CULL_MODE_NONE = 0,
		CULL_MODE_FRONT = 0x00000001,
		CULL_MODE_BACK = 0x00000002,
		CULL_MODE_FRONT_BACK = 0x00000003
	};

	class GraphicsPipeline : public IGpuObject {
	protected:
		FrontFaceMode _frontFace;
		CullMode _cullMode;
	public:
		GraphicsPipeline() 
			: IGpuObject()
		{}
	};
}