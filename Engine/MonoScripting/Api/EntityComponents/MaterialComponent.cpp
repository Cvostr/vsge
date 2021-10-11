#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetMaterial(MaterialComponent* comp, Resource* resource) {
	comp->SetMaterialName(resource->GetName());
}

static bool IsCastShadows(MaterialComponent* comp) {
	return comp->IsCastShadows();
}

static void SetCastShadows(MaterialComponent* comp, bool cast) {
	return comp->SetCastShadows(cast);
}

void VSGE::BindMaterial() {
	mono_add_internal_call("MaterialComponent::i_SetMaterial(ulong,ulong)", SetMaterial);

	mono_add_internal_call("MaterialComponent::i_IsCastShadows(ulong)", IsCastShadows);
	mono_add_internal_call("MaterialComponent::i_SetCastShadows(ulong,bool)", SetCastShadows);
}