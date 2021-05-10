#include "EntityComponents.hpp"
#include <Resources/Resource.hpp>
#include "../ResourcePicker.hpp"

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::DrawMeshComponent(VSGE::MeshComponent* light) {
	DrawResourcePicker(std::string("Mesh"), light->GetResourceReference());
}