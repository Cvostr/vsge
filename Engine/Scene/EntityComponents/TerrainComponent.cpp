#include "TerrainComponent.hpp"

using namespace VSGE;

TerrainComponent::TerrainComponent():
	_width(256),
	_length(256)
{

}

TerrainComponent::~TerrainComponent()
{

}

uint32 TerrainComponent::GetWidth() const
{
	return _width;
}

uint32 TerrainComponent::GetLength() const
{
	return _length;
}

uint32 TerrainComponent::GetVerticesCount() const
{
	return _width * _length;
}

uint32 TerrainComponent::GetIndicesCount() const
{
	return (_width - 1) * (_length - 1) * 2 * 3;
}