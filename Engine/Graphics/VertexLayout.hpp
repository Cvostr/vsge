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

		VL_FORMAT_R32_UINT,
		VL_FORMAT_RG32_UINT,
		VL_FORMAT_RGB32_UINT,
		VL_FORMAT_RGBA32_UINT,
	};

	struct VertexLayoutItem {
		uint32 location;
		uint32 offset;
		uint32 binding;
		VertexLayoutFormat format;
	};

	class VertexLayout {
	private:
		uint32 vertexBindingsCount;
		std::vector<uint32> vertexBindingsSizes;
		std::vector<VertexLayoutItem> items;
	public:

		uint32 GetVertexBindingsCount() const;
		uint32 GetVertexBindingSize(uint32 bindingIndex) const;

		const std::vector<VertexLayoutItem>& GetItems() const;

		void AddBinding(uint32 vertexSize);
		void AddItem(uint32 location, uint32 offset, VertexLayoutFormat format, uint32 binding = 0);

		VertexLayout();
	};
}