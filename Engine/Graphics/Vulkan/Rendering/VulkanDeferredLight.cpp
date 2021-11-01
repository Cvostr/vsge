#include "VulkanDeferredLight.hpp"
#include "VulkanRenderer.hpp"
#include <Resources/DefaultResources.hpp>

#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>

using namespace VSGE;

VulkanDeferredLight::VulkanDeferredLight() {
	_fb_width = 1280;
	_fb_height = 720;
	_camera_index = 0;
	_is_envmap = false;
	_outputFormat = FORMAT_RGBA16F;
	_gbuffer = nullptr;
}

VulkanDeferredLight::~VulkanDeferredLight() {
	SAFE_RELEASE(_deferred_pipeline);
	SAFE_RELEASE(_deferred_pipeline_layout);

	SAFE_RELEASE(_deferred_descriptor);
	SAFE_RELEASE(_deferred_pool);

	SAFE_RELEASE(_deferred_fb);
	SAFE_RELEASE(_deferred_rp);
}

void VulkanDeferredLight::CreateFramebuffer() {
	_deferred_rp = new VulkanRenderPass;
	_deferred_rp->SetClearSize(_fb_width, _fb_height);
	_deferred_rp->PushColorAttachment(_outputFormat);
	_deferred_rp->Create();

	_deferred_fb = new VulkanFramebuffer;
	_deferred_fb->SetStorage(true);
	_deferred_fb->SetSize(_fb_width, _fb_height);
	_deferred_fb->AddAttachment(_outputFormat);
	_deferred_fb->Create(_deferred_rp);
}

void VulkanDeferredLight::CreateDescriptorSet(){
	//Allocate pool
	_deferred_pool = new VulkanDescriptorPool;

	_deferred_descriptor = new VulkanDescriptorSet(_deferred_pool);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, VK_SHADER_STAGE_FRAGMENT_BIT);
	//shadows
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 7, VK_SHADER_STAGE_FRAGMENT_BIT);
	//depth
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8, VK_SHADER_STAGE_FRAGMENT_BIT);
	//brdf lut
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 9, VK_SHADER_STAGE_FRAGMENT_BIT);
	//specular env map
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10, VK_SHADER_STAGE_FRAGMENT_BIT);
	//irradiance env map
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 11, VK_SHADER_STAGE_FRAGMENT_BIT);

	_deferred_pool->Create();
	_deferred_descriptor->Create();

	_deferred_descriptor->WriteDescriptorBuffer(1, VulkanRenderer::Get()->GetCamerasBuffer()->GetCamerasBuffer());
	//write base textures
	VulkanSampler* attachment_sampler = VulkanRenderer::Get()->GetAttachmentSampler();
	_deferred_descriptor->WriteDescriptorImage(7, VulkanRenderer::Get()->GetBlackTexture(), attachment_sampler);
	_deferred_descriptor->WriteDescriptorBuffer(2, (VulkanBuffer*)VulkanRenderer::Get()->GetLightsBuffer()->GetLightsGpuBuffer());
	_deferred_descriptor->WriteDescriptorImage(9, VulkanRenderer::Get()->GetBRDF()->GetTextureLut(), attachment_sampler);

	UnsetIBL();
}

void VulkanDeferredLight::CreatePipeline() {
	_deferred_pipeline_layout = new VulkanPipelineLayout;
	_deferred_pipeline_layout->PushDescriptorSet(_deferred_descriptor);
	_deferred_pipeline_layout->Create();

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	_deferred_pipeline = new VulkanPipeline;
	if(_is_envmap)
		_deferred_pipeline->Create((VulkanShader*)ShaderCache::Get()->GetShader("Deferred_envmap"), _deferred_rp, _vertexLayout, _deferred_pipeline_layout);
	else
		_deferred_pipeline->Create((VulkanShader*)ShaderCache::Get()->GetShader("Deferred"), _deferred_rp, _vertexLayout, _deferred_pipeline_layout);
}

void VulkanDeferredLight::SetLightsBuffer(VulkanBuffer* lights_buffer) {
	_deferred_descriptor->WriteDescriptorBuffer(2, lights_buffer);
}

void VulkanDeferredLight::SetGBuffer(VulkanGBufferRenderer* gbuffer) {
	_gbuffer = gbuffer;

	if (!_deferred_descriptor)
		return;

	VulkanFramebuffer* fb = gbuffer->GetFramebuffer();
	VulkanSampler* attachment_sampler = VulkanRenderer::Get()->GetAttachmentSampler();
	//attachments
	_deferred_descriptor->WriteDescriptorImage(3, (VulkanTexture*)fb->GetColorAttachments()[0], attachment_sampler);
	_deferred_descriptor->WriteDescriptorImage(4, (VulkanTexture*)fb->GetColorAttachments()[1], attachment_sampler);
	_deferred_descriptor->WriteDescriptorImage(5, (VulkanTexture*)fb->GetColorAttachments()[2], attachment_sampler);
	_deferred_descriptor->WriteDescriptorImage(6, (VulkanTexture*)fb->GetColorAttachments()[3], attachment_sampler);
	//depth
	_deferred_descriptor->WriteDescriptorImage(8, (VulkanTexture*)fb->GetDepthAttachment(), attachment_sampler);
}

void VulkanDeferredLight::SetShadowmapper(VulkanShadowmapping* shadowmapping) {
	if (!_deferred_descriptor)
		return;

	VulkanSampler* attachment_sampler = VulkanRenderer::Get()->GetAttachmentSampler();

	_deferred_descriptor->WriteDescriptorImage(7, shadowmapping->GetOutputTexture(), attachment_sampler);
}

void VulkanDeferredLight::UnsetIBL() {
	SetTexture(10, VulkanRenderer::Get()->GetBlackCubeTexture());
	SetTexture(11, VulkanRenderer::Get()->GetWhiteCubeTexture());
}

void VulkanDeferredLight::SetIBL(VulkanTexture* specular, VulkanTexture* irradiance) {
	UnsetIBL();

	SetTexture(10, specular, VulkanRenderer::Get()->GetSpecularIBLSampler());
	SetTexture(11, irradiance);
}

void VulkanDeferredLight::SetTexture(uint32 binding, VulkanTexture* texture, VulkanSampler* sampler) {
	if (!_deferred_descriptor)
		return;

	if(!sampler)
		sampler = VulkanRenderer::Get()->GetAttachmentSampler();

	_deferred_descriptor->WriteDescriptorImage(binding, texture, sampler);
}

void VulkanDeferredLight::RecordCmdbuf(VulkanCommandBuffer* cmdbuf) {
	VulkanMesh* mesh = VulkanRenderer::Get()->GetScreenMesh();

	_deferred_rp->CmdBegin(*cmdbuf, *_deferred_fb);

	DrawSkybox(cmdbuf);

	cmdbuf->BindPipeline(*_deferred_pipeline);
	cmdbuf->SetViewport(0, 0, (float)_fb_width, (float)_fb_height);
	uint32 cam_offset = _camera_index * CAMERA_ELEM_SIZE;
	cmdbuf->BindDescriptorSets(*_deferred_pipeline_layout, 0, 1, _deferred_descriptor, 1, &cam_offset);
	cmdbuf->BindMesh(*mesh, 0);
	cmdbuf->DrawIndexed(6);

	DrawParticles(cmdbuf);

	cmdbuf->EndRenderPass();
}

void VulkanDeferredLight::DrawSkybox(VulkanCommandBuffer* cmdbuf) {
	if (_gbuffer->GetScene()) {
		SceneEnvironmentSettings& env_settings = _gbuffer->GetScene()->GetEnvironmentSettings();
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
					cmdbuf->BindDescriptorSets(*ppl, 0, 1, _gbuffer->GetVertexDescriptorSets()[0], 2, offsets);
					cmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);
					cmdbuf->BindDescriptorSets(*ppl, 3, 1, _deferred_descriptor, 1, &offsets[1]);

					VulkanMesh* cube = (VulkanMesh*)((MeshResource*)GetCubeMesh())->GetMesh();
					cmdbuf->BindMesh(*cube);
					cmdbuf->Draw(cube->GetVerticesCount());
				}
			}
		}
	}
}
void VulkanDeferredLight::DrawParticles(VulkanCommandBuffer* cmdbuf) {
	uint32 writtenParticleTransforms = 0;
	for (uint32 particle_em_i = 0; particle_em_i < _gbuffer->GetParticlesToRender()->size(); particle_em_i++) {
		Entity* entity = (*_gbuffer->GetParticlesToRender())[particle_em_i];
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
			uint32 offset2 = writtenParticleTransforms * sizeof(Mat4);

			cmdbuf->BindDescriptorSets(*ppl, 0, 1, _gbuffer->GetVertexDescriptorSets()[0], 2, offsets1);
			cmdbuf->BindDescriptorSets(*ppl, 2, 1, _gbuffer->GetParticlesDescriptorSet(), 1, &offset2);
			cmdbuf->BindDescriptorSets(*ppl, 3, 1, _deferred_descriptor, 1, offsets1);
			cmdbuf->BindMesh(*mesh);
			uint32 instances_count = particle_emitter->GetAliveParticlesCount();
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount(), instances_count);
			else
				cmdbuf->Draw(mesh->GetVerticesCount(), instances_count);

			uint32 particles_count = particle_emitter->GetAliveParticlesCount();
			writtenParticleTransforms += particles_count;
		}
	}
}

void VulkanDeferredLight::Resize(uint32 width, uint32 height) {
	_fb_width = width;
	_fb_height = height;

	_deferred_rp->SetClearSize(width, height);
	_deferred_fb->Resize(width, height);
}

void VulkanDeferredLight::SetCameraIndex(uint32 camera_index) {
	_camera_index = camera_index;
}

void VulkanDeferredLight::SetEnvmap(bool envmap) {
	_is_envmap = envmap;
}

void VulkanDeferredLight::SetOutputFormat8() {
	_outputFormat = FORMAT_RGBA;
}

VulkanFramebuffer* VulkanDeferredLight::GetFramebuffer() {
	return _deferred_fb;
}

VulkanRenderPass* VulkanDeferredLight::GetRenderPass() {
	return _deferred_rp;
}

TextureFormat VulkanDeferredLight::GetOutputFormat() {
	return _outputFormat;
}

VulkanTexture* VulkanDeferredLight::GetOutputTexture() {
	return (VulkanTexture*)_deferred_fb->GetColorAttachments()[0];
}

VulkanDescriptorSet* VulkanDeferredLight::GetDeferredDescriptorSet() {
	return _deferred_descriptor;
}