#include "VulkanGBufferRenderer.hpp"
#include "../VulkanRAPI.hpp"
#include "VulkanRenderer.hpp"
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Resources/DefaultResources.hpp>

using namespace VSGE;

VulkanGBufferRenderer::VulkanGBufferRenderer() {
	_fb_width = 1280;
	_fb_height = 720;
	_scene = nullptr;

	_camera_index = 0;
	_reverseCull = false;
	_boundPipeline = nullptr;

	renderMode = RENDER_ALL;
}
VulkanGBufferRenderer::~VulkanGBufferRenderer() 
{
	SAFE_RELEASE(m_gbufferFramebuffer)
	SAFE_RELEASE(_gbuffer_renderpass)
}

void VulkanGBufferRenderer::CreateFramebuffer() 
{
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_gbuffer_renderpass = new VulkanRenderPass;
	_gbuffer_renderpass->SetClearSize(_fb_width, _fb_height);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushDepthAttachment(device->GetSuitableDepthFormat());
	_gbuffer_renderpass->Create();

	m_gbufferFramebuffer = new VulkanFramebuffer;
	m_gbufferFramebuffer->SetSize(_fb_width, _fb_height);
	m_gbufferFramebuffer->AddAttachment(FORMAT_RGBA16F); //Color
	m_gbufferFramebuffer->AddAttachment(FORMAT_RGBA16F); //Normal
	m_gbufferFramebuffer->AddAttachment(FORMAT_RGBA16F); //Position
	m_gbufferFramebuffer->AddAttachment(FORMAT_RGBA); //Material
	m_gbufferFramebuffer->AddDepth(GetTextureFormat(device->GetSuitableDepthFormat()));
	m_gbufferFramebuffer->Create(_gbuffer_renderpass);
}

void VulkanGBufferRenderer::Resize(uint32 width, uint32 height) {
	_fb_width = width;
	_fb_height = height;

	_gbuffer_renderpass->SetClearSize(width, height);
	m_gbufferFramebuffer->Resize(_fb_width, _fb_height);
}

void VulkanGBufferRenderer::EnableReverseCull() {
	_reverseCull = true;
}

void VulkanGBufferRenderer::CreateDescriptorSets(VulkanCamerasBuffer* camerasBuffer)
{
	_objects_descr_pool = new VulkanDescriptorPool;

	this->camerasBuffer = camerasBuffer;

	//Create base vertex descriptors set
	_transforms_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_transforms_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT); //Camera
	_transforms_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT); //Transform

	_animations_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_animations_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Animation

	//Create base particles descriptors sets
	_particles_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_particles_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);

	//Create POOL
	_objects_descr_pool->Create();

	// Create all descriptor sets
	_transforms_descriptor_set->Create();
	_transforms_descriptor_set->WriteDescriptorBuffer(0, camerasBuffer->GetCamerasBuffer(), 0, CAMERA_ELEM_SIZE);

	_animations_descriptor_set->Create();
	_particles_descriptor_set->Create();
}

void VulkanGBufferRenderer::SetBuffers(VulkanBuffer* transforms_buffer, VulkanBuffer* anims_buffer, VulkanBuffer* particles_buffer) {
	if (transforms_buffer) {
		_transforms_buffer = transforms_buffer;
		_transforms_descriptor_set->WriteDescriptorBuffer(1, transforms_buffer, 0, sizeof(Mat4));
	}
	if (anims_buffer) {
		_animations_buffer = anims_buffer;
		_animations_descriptor_set->WriteDescriptorBuffer(0, anims_buffer, 0);
	}
	if (particles_buffer) {
		_particles_buffer = particles_buffer;
		_particles_descriptor_set->WriteDescriptorBuffer(0, particles_buffer);
	}
}

void VulkanGBufferRenderer::SetEntitiesToRender(tEntityList& entities, tEntityList& particles) {
	_entities_to_render = &entities;
	_particles_to_render = &particles;
}

tEntityList* VulkanGBufferRenderer::GetEntitiesToRender() {
	return _entities_to_render;
}

tEntityList* VulkanGBufferRenderer::GetParticlesToRender() {
	return _particles_to_render;
}

void VulkanGBufferRenderer::SetScene(Scene* scene) {
	_scene = scene;
}

void VulkanGBufferRenderer::BindPipeline(VulkanCommandBuffer* cmdbuf, VulkanPipeline* pipeline) {
	if (pipeline != _boundPipeline) {
		cmdbuf->BindPipeline(*pipeline);
		cmdbuf->SetViewport(0, 0, _fb_width, _fb_height);
		if (_reverseCull) {
			cmdbuf->SetCullMode(VK_CULL_MODE_BACK_BIT);
		}
		else
			cmdbuf->SetCullMode(VK_CULL_MODE_FRONT_BIT);

		_boundPipeline = pipeline;
	}
}

void VulkanGBufferRenderer::SetRenderMode(WorldRenderMode mode)
{
	this->renderMode = mode;
}

void VulkanGBufferRenderer::RecordCmdBuffer(VulkanCommandBuffer* cmdbuf) {
	uint32 _drawn_bones = 0;
	uint32 _writtenParticleTransforms = 0;
	uint32 drawn_terrains = 0;

	uint32 transformMemoryBlockSize = VulkanRenderer::GetUniformTransformBufferElementSize();

	Camera* camera = camerasBuffer->GetCameraByIndex(_camera_index);

	_gbuffer_renderpass->CmdBegin(*cmdbuf, *m_gbufferFramebuffer);

	if (renderMode == RENDER_NONE) {
		cmdbuf->EndRenderPass();
		return;
	}

	_boundPipeline = nullptr;

	for (uint32 entity_i = 0; entity_i < _entities_to_render->size(); entity_i ++)
	{
		Entity* entity = (*_entities_to_render)[entity_i];

		if (renderMode == RENDER_STATIC_ONLY && !entity->IsStatic()) {
			continue;
		}

		AABB bounding_box = entity->GetAABB();

		MeshComponent* mesh_component = entity->GetComponent<MeshComponent>();
		MaterialComponent* material_component = entity->GetComponent<MaterialComponent>();

		if (!mesh_component && !material_component) 
		{
			TerrainComponent* terrain = entity->GetComponent<TerrainComponent>();
			if (terrain) 
			{
				Vec3 bb_max = bounding_box.GetMax();
				bb_max.y = terrain->GetMaxTerrainHeight();
				bounding_box.Extend(bb_max);
				if (camera->IsVisibleInFrustum(bounding_box)) 
				{
					VulkanTerrainRenderer::DrawTerrain(cmdbuf, terrain, _camera_index, _transforms_descriptor_set, entity_i, _fb_width, _fb_height, _reverseCull);
					//need to rebind pipeline
					_boundPipeline = nullptr;
				}
			}
			continue;
		}

		MeshResource* mesh_resource = mesh_component->GetMeshResource();
		MaterialResource* mat_resource = material_component->GetMaterialResource();

		if (!mesh_resource || !mat_resource)
			continue;

		if (mat_resource->GetState() != RESOURCE_STATE_READY)
			continue;

		//bind material
		MaterialTemplate* templ = mat_resource->GetMaterial()->GetTemplate();
		VulkanPipeline* pipl = (VulkanPipeline*)templ->GetPipeline();
		Material* mat = mat_resource->GetMaterial();
		VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();

		if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();
				
			//Mark mesh resource used in this frame
			mesh_resource->Use();
			//Mark material resource used in this frame
			mat_resource->Use();

			uint32 offsets[2] = { _camera_index * CAMERA_ELEM_SIZE, entity_i * transformMemoryBlockSize };
			uint32 anim_offset = _drawn_bones * sizeof(Mat4);
			_drawn_bones += static_cast<uint32>(mesh->GetBones().size());

			if (!camera->IsVisibleInFrustum(bounding_box))
				continue;
			
			if (templ->GetRenderStage() != RENDER_STAGE_GBUFFER)
				continue;


			BindPipeline(cmdbuf, pipl);
			VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();
			cmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);
			cmdbuf->BindDescriptorSets(*ppl, 0, 1, _transforms_descriptor_set, 2, offsets);
			cmdbuf->BindDescriptorSets(*ppl, 2, 1, _animations_descriptor_set, 1, &anim_offset);
			cmdbuf->BindMesh(*mesh);
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount());
			else
				cmdbuf->Draw(mesh->GetVerticesCount());
		}
	}

	cmdbuf->EndRenderPass();
}

void VulkanGBufferRenderer::CreateBuffers() 
{
	uint32 transformMemoryBlockSize = VulkanRenderer::GetUniformTransformBufferElementSize();

	_transforms_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_transforms_buffer->Create(MAX_OBJECTS_RENDER * transformMemoryBlockSize);

	_animations_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_animations_buffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	_particles_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_particles_buffer->Create(sizeof(Mat4) * MAX_PARTICLES_MATRICES);
}

void VulkanGBufferRenderer::SetCameraIndex(uint32 camera_index) {
	_camera_index = camera_index;
}
Scene* VulkanGBufferRenderer::GetScene() {
	return _scene;
}
VulkanFramebuffer* VulkanGBufferRenderer::GetFramebuffer() {
	return m_gbufferFramebuffer;
}
VulkanRenderPass* VulkanGBufferRenderer::GetRenderPass() {
	return _gbuffer_renderpass;
}
VulkanTexture* VulkanGBufferRenderer::GetAlbedoAttachment() {
	return (VulkanTexture*)m_gbufferFramebuffer->GetColorAttachments()[0];
}
VulkanTexture* VulkanGBufferRenderer::GetNormalAttachment() {
	return (VulkanTexture*)m_gbufferFramebuffer->GetColorAttachments()[1];
}
VulkanTexture* VulkanGBufferRenderer::GetPositionAttachment() {
	return (VulkanTexture*)m_gbufferFramebuffer->GetColorAttachments()[2];
}
VulkanTexture* VulkanGBufferRenderer::GetMaterialsAttachment() {
	return (VulkanTexture*)m_gbufferFramebuffer->GetColorAttachments()[3];
}
VulkanTexture* VulkanGBufferRenderer::GetDepthAttachment() {
	return (VulkanTexture*)m_gbufferFramebuffer->GetDepthAttachment();
}
VulkanBuffer* VulkanGBufferRenderer::GetTransformsBuffer() {
	return _transforms_buffer;
}
VulkanBuffer* VulkanGBufferRenderer::GetAnimationsBuffer() {
	return _animations_buffer;
}
VulkanBuffer* VulkanGBufferRenderer::GetParticlesBuffer() {
	return _particles_buffer;
}

VulkanDescriptorSet* VulkanGBufferRenderer::GetVertexDescriptorSet() {
	return _transforms_descriptor_set;
}
VulkanDescriptorSet* VulkanGBufferRenderer::GetParticlesDescriptorSet() {
	return _particles_descriptor_set;
}
VulkanDescriptorSet* VulkanGBufferRenderer::GetAnimationsDescriptorSet() {
	return _animations_descriptor_set;
}