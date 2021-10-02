#include "ApiBindings.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Resources/Resource.hpp>
#include <Resources/ResourceCache.hpp>

using namespace VSGE;

static MonoString* GetName(void* ptr) {
	MonoScriptingLayer* layer = MonoScriptingLayer::Get();
	return mono_string_new(layer->GetDomain(), ((Resource*)ptr)->GetName().c_str());
}

static ResourceState GetState(void* ptr) {
	return ((Resource*)ptr)->GetState();
}

static ResourceType GetType(void* ptr) {
	return ((Resource*)ptr)->GetResourceType();
}

static Resource* GetPtrByName(MonoString* name) {
	std::string c_name = std::string(mono_string_to_utf8(name));
	return ResourceCache::Get()->GetResource(c_name);
}

void VSGE::BindResourcesApi() {
	mono_add_internal_call("Resource::i_GetName(ulong)", GetName);
	mono_add_internal_call("Resource::i_GetState(ulong)", GetState);
	mono_add_internal_call("Resource::i_GetResourceType(ulong)", GetType);
	mono_add_internal_call("Resource::i_GetPointerByName(ulong,string)", GetPtrByName);
}