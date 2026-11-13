#include "VkGizmos.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>

#include "Graphics/Vulkan/VulkanRAPI.hpp"

using namespace VSGEditor;
using namespace VSGE;

VkGizmos* VkGizmos::_this = nullptr;

#define GIZMO_MAX_ENTITIES 100

VkGizmos::VkGizmos() :
	_object_shader(nullptr),
	_object_pipeline(nullptr),
	_object_pipeline_layout(nullptr),
	_draw_gizmos(false),

	_fb_width(1280),
	_fb_height(720)
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

	// Shader for rendering picked objects
	_object_shader = new VulkanShader;
	_object_shader->AddShaderFromFile("pbr.vert", SHADER_STAGE_VERTEX);
	_object_shader->AddShaderFromFile("mark_gizmo_object.frag", SHADER_STAGE_FRAGMENT);

	_rp = new VulkanRenderPass;
	_rp->SetClearSize(_fb_width, _fb_height);
	_rp->PushColorAttachment(FORMAT_RGBA);
	_rp->Create();
	_rp->SetClearColor(0, Color(0, 0, 0, 0));

	_framebuffer = new VulkanFramebuffer;
	_framebuffer->SetSize(_fb_width, _fb_height);
	_framebuffer->AddAttachment(FORMAT_RGBA); //Color
	_framebuffer->Create(_rp);


	_object_pipeline_layout = new VulkanPipelineLayout;
	// Add common vertex descriptor

	_object_pipeline_layout->PushDescriptorSet(
		VulkanRenderer::Get()->GetMainRenderTarget()->GetGBufferRenderer()->GetVertexDescriptorSet());
	_object_pipeline_layout->PushDescriptorSet(
		VulkanRenderer::Get()->GetMainRenderTarget()->GetGBufferRenderer()->GetVertexDescriptorSet());
	_object_pipeline_layout->PushDescriptorSet(
		VulkanRenderer::Get()->GetMainRenderTarget()->GetGBufferRenderer()->GetAnimationsDescriptorSet());
	// Create pipeline layout
	_object_pipeline_layout->Create();

	// Main pipeline for rendering picked objects
	_object_pipeline = new VulkanPipeline;
	_object_pipeline->SetPolygonMode(POLYGON_MODE_LINE);
	_object_pipeline->SetCullMode(CULL_MODE_NONE);
	_object_pipeline->SetDepthTest(false);
	_object_pipeline->Create(_object_shader, _rp, vertexLayout, _object_pipeline_layout);
	
	// Create buffer for 1 camera
	_cameras_buffer = new VulkanCamerasBuffer;

	// Create transform buffers
	uint32 transformMemoryBlockSize = VulkanRenderer::GetUniformTransformBufferElementSize();
	uint32 alignment = std::max<uint64>((uint64)VSGE::VulkanRAPI::Get()->GetDevice()->GetStorageBufferMinAlignment(), sizeof(Mat4));

	_transforms_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_transforms_buffer->Create(GIZMO_MAX_ENTITIES * transformMemoryBlockSize);

	_animations_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_animations_buffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	// Vulkan Descriptors stuff
	_objects_descr_pool = new VulkanDescriptorPool;

	// Create base vertex descriptors set
	_transforms_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	// Camera
	_transforms_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	// Transform
	_transforms_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT);
	// General + Environment
	_transforms_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, VK_SHADER_STAGE_VERTEX_BIT);

	// Create animatrion descriptors set
	_animations_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_animations_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Animation
	
	// Create POOL
	_objects_descr_pool->Create();

	// Create all descriptor sets and set buffers to them
	_transforms_descriptor_set->Create();
	_transforms_descriptor_set->WriteDescriptorBuffer(0, _cameras_buffer->GetCamerasBuffer(), 0, CAMERA_ELEM_SIZE);
	_transforms_descriptor_set->WriteDescriptorBuffer(1, _transforms_buffer, 0, sizeof(Mat4));

	_animations_descriptor_set->Create();
	_animations_descriptor_set->WriteDescriptorBuffer(0, _animations_buffer, 0);
}

VulkanTexture* VkGizmos::GetOutputTexture()
{
	return (VulkanTexture*)_framebuffer->GetColorAttachments()[0];
}

void VkGizmos::Destroy() {
	SAFE_RELEASE(_object_pipeline);
	SAFE_RELEASE(_object_pipeline_layout);
	SAFE_RELEASE(_object_shader);
}

void VkGizmos::AddPickedEntity(VSGE::Entity* entity) 
{
	if (_picked_entities.size() >= GIZMO_MAX_ENTITIES)
		return;

	auto it = std::find(_picked_entities.begin(), _picked_entities.end(), entity);
	if (it == _picked_entities.end()) {
		_picked_entities.push_back(entity);
	}
}

void VkGizmos::RemovePickedEntity(VSGE::Entity* entity)
{
	_picked_entities.erase(std::remove(_picked_entities.begin(), _picked_entities.end(), entity), _picked_entities.end());
}

void VkGizmos::ClearPickedEntities() {
	_picked_entities.clear();
}

void VkGizmos::UpdateCamera(Camera* cam)
{
	_cameras_buffer->SetCamera(0, cam);
	_cameras_buffer->UpdateGpuBuffer();
}

void VkGizmos::PrepareBuffers() 
{
	uint32 alignment = std::max<uint64>((uint64)VSGE::VulkanRAPI::Get()->GetDevice()->GetStorageBufferMinAlignment(), sizeof(Mat4));
	uint32 alignmentFactor = alignment / sizeof(Mat4);

	Mat4* transforms = new Mat4[_picked_entities.size() * alignmentFactor];

	uint32 writtenTransforms = 0;
	uint32 writtenBones = 0;

	for (auto& entity : _picked_entities) 
	{
		MeshComponent* meshComponent = entity->GetComponent<MeshComponent>();
		if (meshComponent == nullptr)
			continue;

		MeshResource* meshResource = meshComponent->GetMeshResource();
		if (!meshResource)
			continue;

		if (meshResource)
		{
			meshResource->Load();
		}
		else if (!meshComponent->GetResourceReference().GetResourceName().empty() && !meshComponent->GetResourceReference().GetParentResource()->GetName().empty())
		{
			Resource* parent = meshComponent->GetResourceReference().GetParentResource();
			if (parent) {
				parent->Load();
			}
		}

		transforms[(writtenTransforms++) * alignmentFactor] = entity->GetWorldTransform();
		
		VSGE::Mesh* mesh = meshResource->GetMesh();

		if (mesh->GetBones().size() > 0) {
			Entity* rootNode = entity->GetRootSkinningEntity();
			if (rootNode != nullptr) {
				// Discard entity transform from bone offset matrix
				Mat4 entityInvTransform = entity->GetWorldTransform().invert();
				for (uint32 bone_i = 0; bone_i < mesh->GetBones().size(); bone_i++) {
					// Get next bone
					const Bone* bone = &mesh->GetBones()[bone_i];
					// Find entity with name as bone
					Entity* node = rootNode->GetChildEntityWithLabel(bone->GetName());
					if (node != nullptr) {
						// Calculate result matrix
						Mat4 matrix = bone->GetOffsetMatrix() * node->GetWorldTransform() * entityInvTransform;
						// Send skinned matrix to skinning uniform buffer
						_animations_buffer->WriteData(sizeof(Mat4) * writtenBones, sizeof(Mat4), &matrix);
						writtenBones++;
					}
				}
			}
		}
	}

	_transforms_buffer->WriteData(0, sizeof(Mat4) * alignmentFactor * (uint32)_picked_entities.size(), transforms);
	delete[] transforms;
}

void VkGizmos::AppendToCmdbuf(VSGE::VulkanCommandBuffer* cmdbuf) 
{
	uint32 alignment = std::max<uint64>((uint64)VSGE::VulkanRAPI::Get()->GetDevice()->GetStorageBufferMinAlignment(), sizeof(Mat4));
	uint32 entity_index = 0;
	uint32 drawn_bones = 0;

	_rp->CmdBegin(*cmdbuf, *_framebuffer);
	// Bind pipeline at the beginning
	cmdbuf->BindPipeline(*_object_pipeline);
	cmdbuf->SetViewport(0, 0, _fb_width, _fb_height);
	cmdbuf->SetCullMode(VK_CULL_MODE_NONE);
	VulkanPipelineLayout* ppl = _object_pipeline->GetPipelineLayout();

	for (auto& entity : _picked_entities) 
	{
		if (!entity->IsActive())
			continue;

		MeshComponent* meshComponent = entity->GetComponent<MeshComponent>();
		if (meshComponent == nullptr)
			continue;

		MeshResource* meshResource = meshComponent->GetMeshResource();
		if (meshResource == nullptr)
			continue;

		if (meshResource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)meshResource->GetMesh();

			// Mark mesh resource used in this frame
			meshResource->Use();

			uint32 offsets[2] = { 0 * CAMERA_ELEM_SIZE, (entity_index++) * alignment };
			uint32 animOffset = drawn_bones * sizeof(Mat4);
			
			drawn_bones += static_cast<uint32>(mesh->GetBones().size());

			cmdbuf->BindDescriptorSets(*ppl, 0, 1, _transforms_descriptor_set, 2, offsets);
			cmdbuf->BindDescriptorSets(*ppl, 2, 1, _animations_descriptor_set, 1, &animOffset);
			cmdbuf->BindMesh(*mesh);
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount());
			else
				cmdbuf->Draw(mesh->GetVerticesCount());
		}
	}

	cmdbuf->EndRenderPass();
}