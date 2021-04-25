#pragma once

#include <Core/VarTypes/Base.hpp>
#include <vector>
#include <Math/Vertex.hpp>

namespace VSGE {

	enum VertexLayoutFormat {
		VL_FORMAT_RG32_SFLOAT,
		VL_FORMAT_RGB32_SFLOAT,
		VL_FORMAT_RGBA32_SFLOAT,

		VL_FORMAT_RG32_SINT,
		VL_FORMAT_RGB32_SINT,
		VL_FORMAT_RGBA32_SINT,
	};

	struct VertexLayoutItem {
		uint32 location;
		uint32 offset;
		uint32 binding;
		VertexLayoutFormat format;
	};



	struct VertexLayout {
		uint32 vertexBindingsCount;
		std::vector<uint32> vertexSizes;
		std::vector<VertexLayoutItem> items;

		void AddBinding(uint32 vertexSize) {
			vertexSizes.push_back(vertexSize);
			vertexBindingsCount++;
		}

		void AddItem(uint32 location, uint32 offset, VertexLayoutFormat format, uint32 binding = 0) {
			VertexLayoutItem item = {};
			item.binding = binding;
			item.location = location;
			item.offset = offset;
			item.format = format;

			items.push_back(item);
		}

		VertexLayout()
		{
			vertexBindingsCount = 0;
		}

	};
}