#include "VkGizmos.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

using namespace VSGEditor;
using namespace VSGE;

VkGizmos* VkGizmos::_this = nullptr;

VkGizmos::VkGizmos() :
	_object_shader(nullptr),
	_object_pipeline(nullptr),
	_object_pipeline_layout(nullptr),
	_draw_gizmos(false)
{
	_this = this;
}

VkGizmos::~VkGizmos() {
	Destroy();
}

void VkGizmos::Create() {
	VertexLayout vertexLayout;
	vertexLayout.AddBinding(sizeof(Vertex));
	vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	vertexLayout.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	vertexLayout.AddItem(3, offsetof(Vertex, tangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	vertexLayout.AddItem(4, offsetof(Vertex, bitangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	vertexLayout.AddBinding(sizeof(VertexSkinningData));
	vertexLayout.AddItem(5, offsetof(VertexSkinningData, ids), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	vertexLayout.AddItem(6, offsetof(VertexSkinningData, ids[4]), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	vertexLayout.AddItem(7, offsetof(VertexSkinningData, ids[8]), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	vertexLayout.AddItem(8, offsetof(VertexSkinningData, weights), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	vertexLayout.AddItem(9, offsetof(VertexSkinningData, weights[4]), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	vertexLayout.AddItem(10, offsetof(VertexSkinningData, weights[8]), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	vertexLayout.AddItem(11, offsetof(VertexSkinningData, bones_num), VertexLayoutFormat::VL_FORMAT_R32_UINT, 1);

	_object_shader = new VulkanShader;
	_object_shader->AddShaderFromFile("pbr.vert", SHADER_STAGE_VERTEX);
	_object_shader->AddShaderFromFile("mark_gizmo_object.frag", SHADER_STAGE_FRAGMENT);

	VulkanRenderPass* renderpass = 
		VulkanRenderer::Get()->GetMainRenderTarget()->GetDeferredLightRenderer()->GetRenderPass();

	_object_pipeline_layout = new VulkanPipelineLayout;
	//Add common vertex descriptor

	_object_pipeline_layout->PushDescriptorSet(
		VulkanRenderer::Get()->GetMainRenderTarget()->GetGBufferRenderer()->GetVertexDescriptorSets()[0]);
	_object_pipeline_layout->PushDescriptorSet(
		VulkanRenderer::Get()->GetMainRenderTarget()->GetGBufferRenderer()->GetVertexDescriptorSets()[0]);
	_object_pipeline_layout->PushDescriptorSet(
		VulkanRenderer::Get()->GetMainRenderTarget()->GetGBufferRenderer()->GetAnimationsDescriptorSet());
	//Create pipeline layout
	_object_pipeline_layout->Create();

	_object_pipeline = new VulkanPipeline;
	_object_pipeline->SetPolygonMode(POLYGON_MODE_LINE);
	_object_pipeline->SetCullMode(CULL_MODE_NONE);
	_object_pipeline->SetDepthTest(false);
	_object_pipeline->Create(_object_shader, renderpass, vertexLayout, _object_pipeline_layout);
}

void VkGizmos::Destroy() {
	SAFE_RELEASE(_object_pipeline);
	SAFE_RELEASE(_object_pipeline_layout);
	SAFE_RELEASE(_object_shader);
}

void VkGizmos::SetEntitiesToDraw(std::vector<VSGE::Entity*>& entities) {
	_entities_to_draw = &entities;
}
void VkGizmos::AddPickedEntity(VSGE::Entity* entity) {
	_picked_entities.push_back(entity);
}
void VkGizmos::ClearPickedEntities() {
	_picked_entities.clear();
}

void VkGizmos::AppendToCmdbuf(VSGE::VulkanCommandBuffer* cmdbuf) {
	for (auto& entity : _picked_entities) {
		//uint32 entity_index = 
	}
}