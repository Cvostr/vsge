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

	enum PolygonMode {
		POLYGON_MODE_FILL = 0,
		POLYGON_MODE_LINE = 1,
		POLYGON_MODE_POINT = 2
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

	class GraphicsPipeline : public IGpuObject {
	protected:
		FrontFaceMode _frontFace;
		CullMode _cullMode;
		PolygonMode _polygonMode;
		CompareOp _depthOp;
		bool _primitiveRestartEnable;
		bool _depthTest;
		bool _stencilTest;
	public:

		void SetFrontFaceMode(FrontFaceMode mode) {
			_frontFace = mode;
		}

		FrontFaceMode GetFrontFaceMode() {
			return _frontFace;
		}

		void SetCullMode(CullMode mode) {
			_cullMode = mode;
		}

		CullMode GetCullMode() {
			return _cullMode;
		}

		void SetPolygonMode(PolygonMode mode) {
			_polygonMode = mode;
		}

		PolygonMode GetPolygonMode() {
			return _polygonMode;
		}

		CompareOp GetCompareOp() {
			return _depthOp;
		}

		void SetCompareOp(CompareOp compare_op) {
			_depthOp = compare_op;
		}

		void SetDepthTest(bool depthTest){
			_depthTest = depthTest;
		}

		GraphicsPipeline() 
			: IGpuObject(),
			_frontFace(FRONT_FACE_MODE_CW),
			_cullMode(CULL_MODE_NONE),
			_polygonMode(POLYGON_MODE_FILL),
			_depthOp(COMPARE_OP_LESS),
			_primitiveRestartEnable(false),
			_depthTest(false),
			_stencilTest(false)
		{}
	};
}