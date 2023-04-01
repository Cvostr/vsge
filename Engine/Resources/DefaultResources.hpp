#pragma once

#include "ResourceTypes/MaterialResource.hpp"
#include "ResourceTypes/MeshResource.hpp"

namespace VSGE {
	void AddDefaultMeshes();
	void AddDefaultMaterial();

	MeshResource* GetCubeMesh();
	MeshResource* GetSphereMesh();

	MaterialResource* getDefaultMaterialResource();
}