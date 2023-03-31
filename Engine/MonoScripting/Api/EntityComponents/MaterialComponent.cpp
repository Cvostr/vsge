#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetMaterial(MaterialComponent* comp, Resource* resource) {
	comp->SetMaterialId(resource->getId());
}

static Material* GetMaterial(MaterialComponent* comp) {
	return comp->GetMaterial();
}

static bool IsCastShadows(MaterialComponent* comp) {
	return comp->IsCastShadows();
}

static void SetCastShadows(MaterialComponent* comp, bool cast) {
	return comp->SetCastShadows(cast);
}

void VSGE::BindMaterial() {
	MonoScriptingLayer::AddInternalCall("MaterialComponent::i_SetMaterial(ulong,ulong)", SetMaterial);
	MonoScriptingLayer::AddInternalCall("MaterialComponent::i_GetMaterialHandle(ulong)", GetMaterial);

	MonoScriptingLayer::AddInternalCall("MaterialComponent::i_IsCastShadows(ulong)", IsCastShadows);
	MonoScriptingLayer::AddInternalCall("MaterialComponent::i_SetCastShadows(ulong,bool)", SetCastShadows);
}