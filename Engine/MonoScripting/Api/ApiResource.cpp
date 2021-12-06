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
	MonoScriptingLayer::AddInternalCall("Resource::i_GetName(ulong)", GetName);
	MonoScriptingLayer::AddInternalCall("Resource::i_GetState(ulong)", GetState);
	MonoScriptingLayer::AddInternalCall("Resource::i_GetResourceType(ulong)", GetType);
	MonoScriptingLayer::AddInternalCall("Resource::i_GetPointerByName(string)", GetPtrByName);
}