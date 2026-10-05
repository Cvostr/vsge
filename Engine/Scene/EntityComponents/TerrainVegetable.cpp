#include "TerrainComponent.hpp"

#include <mpi/Core/Random.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

TerrainVegetable::TerrainVegetable() :
	_vegetable_coeff(nullptr),
	m_width(1),
	m_height(1)
{
}

TerrainVegetable::~TerrainVegetable() {
	if (_vegetable_coeff)
		delete[] _vegetable_coeff;
}

TerrainVegetable::TerrainVegetable(TerrainVegetable&& other) :
	_mesh_reference(other._mesh_reference),
	_material_reference(other._material_reference),
	m_width(other.m_width),
	m_height(other.m_height)
{
	this->_vegetable_coeff = other._vegetable_coeff;
	other._vegetable_coeff = nullptr;
}

void TerrainVegetable::SetWidth(float width)
{
	this->m_width = width;
}

void TerrainVegetable::SetHeight(float height)
{
	this->m_height = height;
}

uint8* TerrainVegetable::GetCoeffs()
{
	return _vegetable_coeff;
}

void TerrainVegetable::Clear(size_t width, size_t height)
{
	if (_vegetable_coeff)
		delete[] _vegetable_coeff;

	_vegetable_coeff = new uint8[width * height];
	memset(_vegetable_coeff, 0, width * height * sizeof(uint8_t));
}

void TerrainVegetable::GetTransforms(std::vector<Mat4>& out, TerrainComponent* terrain)
{
	uint32 width = terrain->GetWidth();
	uint32 height = terrain->GetLength();
	Vertex* heightmap = terrain->GetVertices();

	if (_vegetable_coeff == nullptr)
		return;

	for (uint32 x = 0; x < width; x++)
	{
		for (uint32 y = 0; y < height; y++)
		{
			uint8_t vegetable_factor = _vegetable_coeff[x * height + y];

			if (vegetable_factor == 0)
				continue;

			Vec3 rotation = Vec3(0, Mpi::Random::random(0, 360), 0);
			Vec3 position = Vec3(x, heightmap[x * height + y].pos.y, y);
			Vec3 scale = Vec3(this->m_width, this->m_height, this->m_width);

			Mat4 scale_mat = GetScaleMatrix(scale);
			Mat4 translation_mat = GetTranslationMatrix(position);

			Mat4 transform = scale_mat * GetRotationMatrix(rotation) * translation_mat;
			out.push_back(transform);
		}
	}
}