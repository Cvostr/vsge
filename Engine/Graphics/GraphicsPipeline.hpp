#pragma once

#include <Core/VarTypes/Base.hpp>
#include <map>
#include "GpuObject.hpp"
#include "Sampler.hpp"

namespace VSGE {

	enum FrontFaceMode {
		FRONT_FACE_MODE_CCW = 0,
		FRONT_FACE_MODE_CW = 1
	};

	enum CullMode {
		CULL_MODE_NONE = 0,
		CULL_MODE_FRONT = 1,
		CULL_MODE_BACK = 2,
		CULL_MODE_FRONT_BACK = 3
	};

	enum PolygonMode {
		POLYGON_MODE_FILL = 0,
		POLYGON_MODE_LINE = 1,
		POLYGON_MODE_POINT = 2
	};

	enum PrimitiveTopology {
		PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
		PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
		PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
		PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
		PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
		PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,
		PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
		PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
		PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
		PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
		PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
	};

	enum ColorBits {
		COLOR_R = 1,
		COLOR_G = 2,
		COLOR_B = 4,
		COLOR_A = 8,
		COLOR_ALL = 15
	};

	enum BlendFactor {
		BLEND_FACTOR_ZERO = 0,
		BLEND_FACTOR_ONE = 1,
		BLEND_FACTOR_SRC_COLOR = 2,
		BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 3,
		BLEND_FACTOR_DST_COLOR = 4,
		BLEND_FACTOR_ONE_MINUS_DST_COLOR = 5,
		BLEND_FACTOR_SRC_ALPHA = 6,
		BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 7,
		BLEND_FACTOR_DST_ALPHA = 8,
		BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 9
	};

	typedef struct BlendAttachmentDesc {
		bool _blending;
		uint32 _colorMask;
		BlendFactor _srcColor;
		BlendFactor _dstColor;
		BlendFactor _srcAlpha;
		BlendFactor _dstAlpha;

		BlendAttachmentDesc() {
			_blending = true;
			_colorMask = COLOR_R | COLOR_G | COLOR_B | COLOR_A;

			_srcColor = BLEND_FACTOR_ONE;
			_dstColor = BLEND_FACTOR_ZERO;

			_srcAlpha = BLEND_FACTOR_ONE;
			_dstAlpha = BLEND_FACTOR_ZERO;
		}

	}BlendAttachmentDesc;

	typedef std::map<uint32, BlendAttachmentDesc> BlendAttachmentDescsMap;

	class GraphicsPipeline : public IGpuObject {
	protected:
		BlendAttachmentDescsMap _blendDescs;
		PrimitiveTopology _primitiveTopology;
		FrontFaceMode _frontFace;
		CullMode _cullMode;
		PolygonMode _polygonMode;
		CompareOp _depthOp;
		bool _primitiveRestartEnable;
		bool _depthTest;
		bool _stencilTest;
		bool _depthClamp;
	public:

		void SetPrimitiveTopology(PrimitiveTopology topology) {
			_primitiveTopology = topology;
		}

		PrimitiveTopology GetPrimitiveTopology() {
			return _primitiveTopology;
		}

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

		void SetDepthClamp(bool depthClamp){
			_depthClamp = depthClamp;
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

		void SetBlendingDescs(const BlendAttachmentDescsMap& map) {
			_blendDescs = map;
		}

		GraphicsPipeline() 
			: IGpuObject(),
			_primitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
			_frontFace(FRONT_FACE_MODE_CW),
			_cullMode(CULL_MODE_NONE),
			_polygonMode(POLYGON_MODE_FILL),
			_depthOp(COMPARE_OP_LESS),
			_primitiveRestartEnable(false),
			_depthTest(false),
			_depthClamp(false),
			_stencilTest(false)
		{}
	};
}