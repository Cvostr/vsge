#include "Resource.hpp"
#include "ResourceCache.hpp"
#include <Core/Time.hpp>
#include <mpi/Filesystem/File.hpp>
#include <fstream>

using namespace VSGE;

#define DEFAULT_RESOURCE_PATH_STRING "@d"

Resource::Resource() :
	m_parent(nullptr),
	m_resourceState(RESOURCE_STATE_UNLOADED),
	_memoryUse(0),
	_lastUseTime(0),
	_loadedData(nullptr),
	_flags(0)
{
}

Resource::~Resource()
{
	FreeLoadedData();
	// TODO: add more
}

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

bool Resource::IsReady() const 
{
	return m_resourceState == RESOURCE_STATE_READY; 
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

std::vector<Resource*>& Resource::GetSubresources() 
{
	return _subresources;
}

Resource* Resource::GetSubresourceWithName(const std::string& name) const
{
	for (auto& resource : _subresources)
	{
		if (resource->GetName() == name)
			return resource;
	}

	return nullptr;
}

bool Resource::IsUnloaded() const
{
	return m_resourceState == RESOURCE_STATE_UNLOADED; 
}

byte* Resource::GetLoadedData() {
	return _loadedData;
}

void Resource::Use() {
	_lastUseTime = TimePerf::Get()->GetCurrentTime();
}

uint64 Resource::GetLastUseTime() const
{
	return _lastUseTime;
}

void Resource::Load(bool just_read) 
{
	if (IsUnloaded()) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(this);
	}
}

void Resource::ReadData()
{
	SetState(RESOURCE_STATE_LOADING);
	//Declare blob loader stream
	std::ifstream stream;
	//Get absolute path to blob file
	std::string file_path = _description.file_path;
	//Check, if size explicitly specified
	if (_description.size == 0) {
		//Если размер не указан (равен 0)
		Mpi::File file(file_path);
		_description.size = static_cast<uint32>(file.getFileSize());
	}

	stream.open(file_path, std::ofstream::binary);
	stream.seekg(static_cast<long>(_description.offset));

	if (!stream.is_open()) {
		SetState(RESOURCE_STATE_LOADING_FAILED);
		throw std::runtime_error("Error loading resource: " + GetName());
	}

	_loadedData = new byte[_description.size];
	stream.read(reinterpret_cast<char*>(_loadedData), _description.size);
	stream.close();

	//Set resource state to loaded
	SetState(RESOURCE_STATE_LOADED);
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

bool Resource::IsDefault() const 
{
	return (_flags & RESOURCE_FLAG_DEFAULT) == RESOURCE_FLAG_DEFAULT;
}

void Resource::SetDefault() 
{
	_description.file_path = DEFAULT_RESOURCE_PATH_STRING;
	_flags |= RESOURCE_FLAG_DEFAULT;
}

uint32 Resource::GetFlags() const
{
	return _flags;
}

void Resource::SetFlags(uint32 flags)
{
	_flags = flags;
}