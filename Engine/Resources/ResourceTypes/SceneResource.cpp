#include "SceneResource.hpp"
#include <Core/Memory.hpp>

using namespace VSGE;

void SceneResource::Prepare() {
	SceneLayer::Get()->LoadSceneBinary(_loadedData, _description.size, _load_slot);

	FreeLoadedData();
	SetState(RESOURCE_STATE_UNLOADED);
}

void SceneResource::LoadAsync(uint32 slot) {
	_load_slot = slot;
	Load();
}