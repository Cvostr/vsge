#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <string>

namespace VSGE {
	class MeshComponent : public IEntityComponent {
	private:
	public:
		void SetMeshResource();
		void SetMesnName(const std::string& mesh);
	};
}