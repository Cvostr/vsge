#include "VertexLayout.hpp"

using namespace VSGE;

uint32 VertexLayout::GetVertexBindingsCount() const
{
	return vertexBindingsCount;
}

uint32 VertexLayout::GetVertexBindingSize(uint32 bindingIndex) const
{
	return vertexBindingsSizes[bindingIndex];
}

const std::vector<VertexLayoutItem>& VertexLayout::GetItems() const
{
	return items;
}

void VertexLayout::AddBinding(uint32 vertexSize)
{
	vertexBindingsSizes.push_back(vertexSize);
	vertexBindingsCount++;
}

void VertexLayout::AddItem(uint32 location, uint32 offset, VertexLayoutFormat format, uint32 binding)
{
	VertexLayoutItem item = {};
	item.binding = binding;
	item.location = location;
	item.offset = offset;
	item.format = format;

	items.push_back(item);
}

VertexLayout::VertexLayout()
{
	vertexBindingsCount = 0;
}