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

float TerrainVegetable::GetWidth() const
{
	return m_width;
}

float TerrainVegetable::GetHeight() const
{
	return m_height;
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

			// Vertex of heightmap
			Vertex* height_vertex = &heightmap[x * height + y];
			Vec3 N = height_vertex->normal;

			for (uint8 fx = 0; fx < vegetable_factor; fx++) 
			{
				for (uint8 fz = 0; fz < vegetable_factor; fz++) 
				{
					float posX = x + (static_cast<float>(fx)) / vegetable_factor;
					float posZ = y + (static_cast<float>(fz)) / vegetable_factor;

					Vec3 rotation = Vec3(0, Mpi::Random::random(0, 360), 0);
					Vec3 position = Vec3(posX, height_vertex->pos.y, posZ);
					Vec3 scale = Vec3(this->m_width, this->m_height, this->m_width);

					Mat4 scale_mat = GetScaleMatrix(scale);
					Mat4 translation_mat = GetTranslationMatrix(position);

					// rotate vegetable over heightmap
					Vec3 FTemp = (abs(N.y) > 0.99f) ? Vec3(1.0f, 0.0f, 0.0f) : Vec3(0.0f, 0.0f, 1.0f);
					Vec3 R = FTemp.Cross(N).GetNormalized();
					Vec3 F = N.Cross(R);
					// matrix for rotation over heightmap
					Mat4 rotN = Mat4(Vec4(R, 0.f), Vec4(N, 0.f), Vec4(F, 0.f), Vec4(0.f, 0.f, 0.f, 1.f));

					// resulting tranform vector
					Mat4 transform = scale_mat * GetRotationMatrix(rotation) * rotN * translation_mat;
					out.push_back(transform);
				}
			}
		}
	}
}