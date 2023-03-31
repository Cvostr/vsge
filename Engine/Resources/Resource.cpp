#include "Resource.hpp"
#include "ResourceCache.hpp"
#include <Core/Time.hpp>

using namespace VSGE;

const Guid& Resource::getId() const
{
	return m_id;
}

void Resource::SetId(const Guid& id)
{
	m_id = id;
}

const std::string& Resource::GetName() const 
{
	return m_name; 
}

void Resource::SetName(const std::string& name)
{ 
	m_name = name;
}

const ResourceState& Resource::GetState() const
{ 
	return m_resourceState; 
}

void Resource::SetState(const ResourceState& State)
{ 
	m_resourceState = State;
}

Resource* Resource::GetParent() {
	return m_parent;
}

void Resource::SetParent(Resource* resource) {
	m_parent = resource;
}

void Resource::SetDataDescription(const DataDescription& desc) {
	_description = desc;
}

const DataDescription& Resource::GetDataDescription() {
	return _description;
}

std::vector<Resource*>& Resource::GetSubresources() {
	return _subresources;
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