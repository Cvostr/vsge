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

	_camera_index = 0;
	_reverseCull = false;
	_boundPipeline = nullptr;
}
VulkanGBufferRenderer::~VulkanGBufferRenderer() {
	SAFE_RELEASE(_gbuffer_fb)
	SAFE_RELEASE(_gbuffer_renderpass)
}

void VulkanGBufferRenderer::CreateFramebuffer() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_gbuffer_renderpass = new VulkanRenderPass;
	_gbuffer_renderpass->SetClearSize(_fb_width, _fb_height);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA16F);
	_gbuffer_renderpass->PushColorAttachment(FORMAT_RGBA);
	_gbuffer_renderpass->PushDepthAttachment(device->GetSuitableDepthFormat());
	_gbuffer_renderpass->Create();

	_gbuffer_fb = new VulkanFramebuffer;
	_gbuffer_fb->SetSize(_fb_width, _fb_height);
	_gbuffer_fb->AddAttachment(FORMAT_RGBA); //Color
	_gbuffer_fb->AddAttachment(FORMAT_RGBA16F); //Normal
	_gbuffer_fb->AddAttachment(FORMAT_RGBA16F); //Position
	_gbuffer_fb->AddAttachment(FORMAT_RGBA); //Material
	_gbuffer_fb->AddDepth(GetTextureFormat(device->GetSuitableDepthFormat()));
	_gbuffer_fb->Create(_gbuffer_renderpass);
}

void VulkanGBufferRenderer::Resize(uint32 width, uint32 height) {
	_fb_width = width;
	_fb_height = height;

	_gbuffer_renderpass->SetClearSize(width, height);
	_gbuffer_fb->Resize(_fb_width, _fb_height);
}

void VulkanGBufferRenderer::EnableReverseCull() {
	_reverseCull = true;
}

void VulkanGBufferRenderer::CreateDescriptorSets() {
	_objects_descr_pool = new VulkanDescriptorPool;

	//Create base vertex descriptors sets
	for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
		VulkanDescriptorSet* set = new VulkanDescriptorSet(_objects_descr_pool);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT); //Camera
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT); //Transform
		_vertex_descriptor_sets.push_back(set);
	}

	_animations_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_animations_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Animation

	//Create base particles descriptors sets
	_particles_descriptor_set = new VulkanDescriptorSet(_objects_descr_pool);
	_particles_descriptor_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);

	//Create POOL
	_objects_descr_pool->Create();

	for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
		VulkanDescriptorSet* set = _vertex_descriptor_sets[desc_i];
		set->Create();
		set->WriteDescriptorBuffer(0, VulkanRenderer::Get()->GetCamerasBuffer()->GetCamerasBuffer());
	}

	_animations_descriptor_set->Create();
	_particles_descriptor_set->Create();
}

void VulkanGBufferRenderer::SetBuffers(VulkanBuffer* transforms_buffer, VulkanBuffer* anims_buffer, VulkanBuffer* particles_buffer) {
	if (transforms_buffer) {
		_transforms_buffer = transforms_buffer;
		for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
			VulkanDescriptorSet* set = _vertex_descriptor_sets[desc_i];
			set->WriteDescriptorBuffer(1, transforms_buffer, desc_i * 1024 * 64, sizeof(Mat4) * 1024);
		}
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

void VulkanGBufferRenderer::RecordCmdBuffer(VulkanCommandBuffer* cmdbuf) {
	uint32 _drawn_bones = 0;
	uint32 _writtenParticleTransforms = 0;
	uint32 drawn_terrains = 0;

	Camera* camera = VulkanRenderer::Get()->GetCamerasBuffer()->GetCameraByIndex(_camera_index);

	VulkanTerrainRenderer* terrain_renderer = VulkanRenderer::Get()->GetTerrainRenderer();

	_gbuffer_renderpass->CmdBegin(*cmdbuf, *_gbuffer_fb);

	Scene* scene = VulkanRenderer::Get()->GetScene();
	if (scene) {
		SceneEnvironmentSettings& env_settings = scene->GetEnvironmentSettings();
		if (env_settings._skybox_material.IsResourceSpecified()) {
			MaterialResource* resource = (MaterialResource*)env_settings._skybox_material.GetResource();
			if (resource) {

				if (resource->IsUnloaded()) {
					resource->Load();
				}

				if (resource->IsReady()) {
					resource->Use();
					Material* mat = resource->GetMaterial();
					VulkanRenderer::Get()->UpdateMaterialDescrSet(mat);
					VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();
					MaterialTemplate* templ = resource->GetMaterial()->GetTemplate();
					VulkanPipeline* pipl = (VulkanPipeline*)templ->GetPipeline();
					VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();

					cmdbuf->BindPipeline(*pipl);
					cmdbuf->SetViewport(0, 0, _fb_width, _fb_height);
					cmdbuf->SetCullMode(VK_CULL_MODE_NONE);
					uint32 offsets[2] = { _camera_index * CAMERA_ELEM_SIZE, 0 };
					cmdbuf->BindDescriptorSets(*ppl, 0, 1, _vertex_descriptor_sets[0], 2, offsets);
					cmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);

					VulkanMesh* cube = (VulkanMesh*)((MeshResource*)GetCubeMesh())->GetMesh();
					cmdbuf->BindMesh(*cube);
					cmdbuf->Draw(cube->GetVerticesCount());
				}
			}
		}
	}

	_boundPipeline = nullptr;
	for (uint32 e_i = 0; e_i < _entities_to_render->size(); e_i++) {
		Entity* entity = (*_entities_to_render)[e_i];

		AABB bounding_box = entity->GetAABB();

		if (!camera->IsVisibleInFrustum(bounding_box))
			continue;

		MeshComponent* mesh_component = entity->GetComponent<MeshComponent>();
		MaterialComponent* material_component = entity->GetComponent<MaterialComponent>();

		if (!mesh_component && !material_component) {
			TerrainComponent* terrain = entity->GetComponent<TerrainComponent>();
			if (terrain) {
				terrain_renderer->DrawTerrain(cmdbuf, drawn_terrains++, e_i);
			}
			continue;
		}

		MeshResource* mesh_resource = mesh_component->GetMeshResource();
		MaterialResource* mat_resource = material_component->GetMaterialResource();

		if (mat_resource->GetState() != RESOURCE_STATE_READY)
			continue;

		//bind material
		MaterialTemplate* templ = mat_resource->GetMaterial()->GetTemplate();
		VulkanPipeline* pipl = (VulkanPipeline*)templ->GetPipeline();
		Material* mat = mat_resource->GetMaterial();
		VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();

		BindPipeline(cmdbuf, pipl);
		VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();
		cmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);

		if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();
			//Mark mesh resource used in this frame
			mesh_resource->Use();
			//Mark material resource used in this frame
			mat_resource->Use();

			uint32 offsets[2] = { _camera_index * CAMERA_ELEM_SIZE, e_i * UNI_ALIGN % 65535 };
			uint32 anim_offset = _drawn_bones * sizeof(Mat4);
			_drawn_bones += mesh->GetBones().size();
			int vertexDescriptorID = (e_i * UNI_ALIGN) / 65535;

			cmdbuf->BindDescriptorSets(*ppl, 0, 1, _vertex_descriptor_sets[vertexDescriptorID], 2, offsets);
			cmdbuf->BindDescriptorSets(*ppl, 2, 1, _animations_descriptor_set, 1, &anim_offset);
			cmdbuf->BindMesh(*mesh);
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount());
			else
				cmdbuf->Draw(mesh->GetVerticesCount());
		}
	}

	for (uint32 particle_em_i = 0; particle_em_i < _particles_to_render->size(); particle_em_i++) {
		Entity* entity = (*_particles_to_render)[particle_em_i];
		ParticleEmitterComponent* particle_emitter = entity->GetComponent<ParticleEmitterComponent>();

		if (!particle_emitter->IsSimulating())
			continue;

		MeshResource* mesh_resource = entity->GetComponent<MeshComponent>()->GetMeshResource();
		MaterialResource* mat_resource = entity->GetComponent<MaterialComponent>()->GetMaterialResource();

		//bind material
		MaterialTemplate* templ = mat_resource->GetMaterial()->GetTemplate();
		VulkanPipeline* pipl = (VulkanPipeline*)templ->GetPipeline();
		Material* mat = mat_resource->GetMaterial();
		VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();

		cmdbuf->BindPipeline(*pipl);
		cmdbuf->SetViewport(0, 0, _fb_width, _fb_height);
		cmdbuf->SetCullMode(VK_CULL_MODE_NONE);
		VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();
		cmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);

		if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();
			//Mark mesh resource used in this frame
			mesh_resource->Use();
			//Mark material resource used in this frame
			mat_resource->Use();

			uint32 offsets1[2] = { _camera_index * CAMERA_ELEM_SIZE, 0 };
			uint32 offset2 = _writtenParticleTransforms * sizeof(Mat4);

			cmdbuf->BindDescriptorSets(*ppl, 0, 1, _vertex_descriptor_sets[0], 2, offsets1);
			cmdbuf->BindDescriptorSets(*ppl, 2, 1, _particles_descriptor_set, 1, &offset2);
			cmdbuf->BindMesh(*mesh);
			uint32 instances_count = particle_emitter->GetAliveParticlesCount();
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount(), instances_count);
			else
				cmdbuf->Draw(mesh->GetVerticesCount(), instances_count);

			uint32 particles_count = particle_emitter->GetAliveParticlesCount();
			_writtenParticleTransforms += particles_count;
		}
	}

	cmdbuf->EndRenderPass();
}

void VulkanGBufferRenderer::CreateBuffers() {
	_transforms_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_transforms_buffer->Create(MAX_OBJECTS_RENDER * UNI_ALIGN);

	_animations_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_animations_buffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	_particles_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_particles_buffer->Create(sizeof(Mat4) * MAX_PARTICLES_MATRICES);
}

void VulkanGBufferRenderer::SetCameraIndex(uint32 camera_index) {
	_camera_index = camera_index;
}
VulkanFramebuffer* VulkanGBufferRenderer::GetFramebuffer() {
	return _gbuffer_fb;
}
VulkanRenderPass* VulkanGBufferRenderer::GetRenderPass() {
	return _gbuffer_renderpass;
}
VulkanTexture* VulkanGBufferRenderer::GetAlbedoAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[0];
}
VulkanTexture* VulkanGBufferRenderer::GetNormalAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[1];
}
VulkanTexture* VulkanGBufferRenderer::GetPositionAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[2];
}
VulkanTexture* VulkanGBufferRenderer::GetMaterialsAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetColorAttachments()[3];
}
VulkanTexture* VulkanGBufferRenderer::GetDepthAttachment() {
	return (VulkanTexture*)_gbuffer_fb->GetDepthAttachment();
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

std::vector<VulkanDescriptorSet*>& VulkanGBufferRenderer::GetVertexDescriptorSets() {
	return _vertex_descriptor_sets;
}
VulkanDescriptorSet* VulkanGBufferRenderer::GetAnimationsDescriptorSet() {
	return _animations_descriptor_set;
}