#include "VkGizmos.hpp"

using namespace VSGEditor;
using namespace VSGE;

VkGizmos* VkGizmos::_this = nullptr;

VkGizmos::VkGizmos() :
	_object_shader(nullptr),
	_object_pipeline(nullptr),
	_object_pipeline_layout(nullptr)
{
	_this = this;
}

VkGizmos::~VkGizmos() {

}

void VkGizmos::Create() {
	_object_shader = new VulkanShader;
	_object_shader->AddShaderFromFile("pbr/pbr.vert", SHADER_STAGE_VERTEX);
	_object_shader->AddShaderFromFile("editor/mark_gizmo_object.frag", SHADER_STAGE_FRAGMENT);
}

void VkGizmos::Destroy() {

}

void VkGizmos::AddPickedEntity(VSGE::Entity* entity) {
	_picked_entities.push_back(entity);
}
void VkGizmos::ClearPickedEntities() {
	_picked_entities.clear();
}