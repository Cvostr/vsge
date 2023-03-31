#pragma once

#include "ResourceTypes/MaterialResource.hpp"

namespace VSGE {
	void AddDefaultMeshes();
	void AddDefaultMaterial();

	void* GetCubeMesh();
	void* GetSphereMesh();

	MaterialResource* getDefaultMaterialResource();
}