#include "Resource.hpp"
#include "ResourceCache.hpp"
#include <Core/Time.hpp>

using namespace VSGE;

Resource* Resource::GetParent() {
	return _parent;
}

void Resource::SetParent(Resource* resource) {
	_parent = resource;
}

void Resource::SetDataDescription(const DataDescription& desc) {
	_description = desc;
}

const DataDescription& Resource::GetDataDescription() {
	return _description;
}

byte* Resource::GetLoadedData() {
	return _loadedData;
}

void Resource::SetLoadedData(byte* data) {
	_loadedData = data;
}

void Resource::Use() {
	_lastUseTime = TimePerf::Get()->GetCurrentTime();
}

void Resource::Load(bool just_read) {
	if (IsUnloaded()) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(this);
	}
}

void Resource::FreeLoadedData() {
	SAFE_RELEASE_ARR(_loadedData)
}

void Resource::Release(){
	if(_loadedData)
		FreeLoadedData();

	OnRelease();
	SetState(RESOURCE_STATE_UNLOADED);
	_lastUseTime = 0;
}

bool Resource::IsDefault() {
	return _description.file_path == "@d";
}

void Resource::SetDefault() {
	_description.file_path = "@d";
}