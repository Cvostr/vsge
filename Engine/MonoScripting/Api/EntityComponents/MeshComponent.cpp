#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetMesh(MeshComponent* ptr, Resource* res) {
	ptr->SetMeshResource((MeshResource*)res);
}

void VSGE::BindMesh() {
	mono_add_internal_call("MeshComponent::i_SetMesh(ulong,ulong)", SetMesh);
}