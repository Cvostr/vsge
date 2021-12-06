#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetMesh(MeshComponent* ptr, Resource* res) {
	ptr->SetMeshResource((MeshResource*)res);
}

void VSGE::BindMesh() {
	MonoScriptingLayer::AddInternalCall("MeshComponent::i_SetMesh(ulong,ulong)", SetMesh);
}