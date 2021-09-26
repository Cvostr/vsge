#include "ApiBindings.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Resources/Resource.hpp>

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

void VSGE::BindResourcesApi() {
	mono_add_internal_call("Resource::i_GetName(ulong)", GetName);
	mono_add_internal_call("Resource::i_GetState(ulong)", GetState);
	mono_add_internal_call("Resource::i_GetResourceType(ulong)", GetType);
}