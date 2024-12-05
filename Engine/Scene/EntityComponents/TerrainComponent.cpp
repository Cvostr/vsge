#include "TerrainComponent.hpp"

using namespace VSGE;

TerrainComponent::TerrainComponent():
	_width(256),
	_length(256),
	_castShadows(true)
{
	_mesh_dirty = false;
	_texturemaps_dirty = false;
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
void TerrainComponent::SetWidth(uint32 width)
{
	_width = width;
}
void TerrainComponent::SetLength(uint32 length)
{
	_length = length;
}

uint32 TerrainComponent::GetVerticesCount() const
{
	return _width * _length;
}

uint32 TerrainComponent::GetIndicesCount() const
{
	return (_width - 1) * (_length - 1) * 2 * 3;
}

bool TerrainComponent::IsShadowCastEnabled() const
{
	return _castShadows;
}

void TerrainComponent::SetShadowCastEnabled(bool cast)
{
	this->_castShadows = cast;
}

void TerrainComponent::OnUpdate()
{

}

void TerrainComponent::Serialize(ByteSerialize& serializer)
{
	serializer.Serialize(_width);
	serializer.Serialize(_length);
	serializer.Serialize(_castShadows);
}

void TerrainComponent::Deserialize(ByteSolver& solver)
{
	_width = solver.GetValue<uint32>();
	_length = solver.GetValue<uint32>();
	_castShadows = solver.GetValue<bool>();
}