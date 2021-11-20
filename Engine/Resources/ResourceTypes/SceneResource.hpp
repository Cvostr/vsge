#pragma once

#include "../Resource.hpp"
#include <Scene/SceneLayer.hpp>

namespace VSGE {
	class SceneResource : public Resource {
	private:
		uint32 _load_slot;
	public:

		SceneResource() {

		}

		~SceneResource() {

		}

		void OnRelease() {

		}

		void Prepare();

		void LoadAsync(uint32 slot = 0);

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_SCENE)

	};
}