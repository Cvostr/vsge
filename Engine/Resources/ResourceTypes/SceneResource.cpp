#include "SceneResource.hpp"

using namespace VSGE;

void SceneResource::Prepare() {
	//check header
	bool is_binary = true;
	bool is_yaml = true;
	char* binary_header = "vsgebscene";
	char* yaml_header = "Scene: Untitled";
	for (uint32 i = 0; i < 10; i++) {
		if (_loadedData[i] != binary_header[i])
			is_binary = false;
	}
	for (uint32 i = 0; i < 15; i++) {
		if (_loadedData[i] != yaml_header[i])
			is_yaml = false;
	}

	if (is_binary)
		SceneLayer::Get()->LoadSceneBinary(_loadedData, _description.size, _load_slot);
	if (is_yaml) {
		byte* terminated = new byte[_description.size + 1];
		memcpy(terminated, _loadedData, _description.size);
		terminated[_description.size] = 0;
		SceneLayer::Get()->LoadSceneYAML(terminated, _load_slot);
		delete[] terminated;
	}
	FreeLoadedData();
	SetState(RESOURCE_STATE_UNLOADED);
}

void SceneResource::LoadAsync(uint32 slot) {
	_load_slot = slot;
	Load();
}