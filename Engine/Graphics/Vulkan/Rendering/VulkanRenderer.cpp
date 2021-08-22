#include "VulkanRenderer.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanShader.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Resources/DefaultResources.hpp>
#include <Math/MatrixCamera.hpp>

using namespace VSGE;

VulkanRenderer* VulkanRenderer::_this = nullptr;

void VulkanRenderer::SetupRenderer() {
	//--------------------Shaders-----------------------
	ShaderStorage::Get()->LoadShaderBundle("../Shaders/shaders.bundle", "../Shaders/shaders.map");
	VulkanShader* pbr = new VulkanShader;
	pbr->AddShaderFromFile("pbr.vert", SHADER_STAGE_VERTEX);
	pbr->AddShaderFromFile("pbr.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(pbr, "PBR");

	VulkanShader* deferred_light = new VulkanShader;
	deferred_light->AddShaderFromFile("deferred.vert", SHADER_STAGE_VERTEX);
	deferred_light->AddShaderFromFile("deferred_pbr.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(deferred_light, "Deferred");

	VulkanShader* particle = new VulkanShader;
	particle->AddShaderFromFile("particle.vert", SHADER_STAGE_VERTEX);
	particle->AddShaderFromFile("particle.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(particle, "Particle");

	VulkanShader* skybox = new VulkanShader;
	skybox->AddShaderFromFile("skybox.vert", SHADER_STAGE_VERTEX);
	skybox->AddShaderFromFile("skybox.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(skybox, "Skybox");

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();
	//--------------------Framebuffers----------------
	mGBufferPass = new VulkanRenderPass;
	mGBufferPass->SetClearSize(mOutputWidth, mOutputHeight);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA16F);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA16F);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA);
	mGBufferPass->PushDepthAttachment(device->GetSuitableDepthFormat());
	mGBufferPass->Create();

	mGBuffer = new VulkanFramebuffer;
	mGBuffer->SetSize(mOutputWidth, mOutputHeight);
	mGBuffer->AddAttachment(FORMAT_RGBA); //Color
	mGBuffer->AddAttachment(FORMAT_RGBA16F); //Normal
	mGBuffer->AddAttachment(FORMAT_RGBA16F); //Position
	mGBuffer->AddAttachment(FORMAT_RGBA); //Material
	mGBuffer->AddDepth(GetTextureFormat(device->GetSuitableDepthFormat()));
	mGBuffer->Create(mGBufferPass);

	mOutputPass = new VulkanRenderPass;
	mOutputPass->SetClearSize(mOutputWidth, mOutputHeight);
	mOutputPass->PushColorAttachment(FORMAT_RGBA);
	mOutputPass->Create();

	mOutputBuffer = new VulkanFramebuffer;
	mOutputBuffer->SetSize(mOutputWidth, mOutputHeight);
	mOutputBuffer->AddAttachment(FORMAT_RGBA);
	mOutputBuffer->Create(mOutputPass);
	

	mOutput = mOutputBuffer->GetColorAttachments()[0];
	
	//-----------------------Prepare semaphores-------------------
	mBeginSemaphore = new VulkanSemaphore;
	mBeginSemaphore->Create();

	mGBufferSemaphore = new VulkanSemaphore;
	mGBufferSemaphore->Create();

	mEndSemaphore = new VulkanSemaphore;
	mEndSemaphore->Create();

	mShadowmappingEndSemaphore = new VulkanSemaphore;
	mShadowmappingEndSemaphore->Create();

	mShadowprocessingEndSemaphore = new VulkanSemaphore;
	mShadowprocessingEndSemaphore->Create();
	
	//---------------------Buffers--------------------------------
	mCameraShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mCameraShaderBuffer->Create(MAX_CAMERAS * UNI_ALIGN);

	mTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mTransformsShaderBuffer->Create(MAX_OBJECTS_RENDER * UNI_ALIGN);

	mAnimationTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	mAnimationTransformsShaderBuffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	mParticlesTransformShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	mParticlesTransformShaderBuffer->Create(sizeof(Mat4) * MAX_PARTICLES_MATRICES);

	_lightsBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_lightsBuffer->Create(200 * 64 + 16);

	//---------------------Meshes----------------------------------
	mSpriteMesh = new VulkanMesh;
	Vertex plane_verts[] = {
		// positions              // texture coords
		Vertex(Vec3(1.0f,  1.0f, 0.0f),   Vec2(1.0f, 1.0f),   Vec3(0, 0, 1)),   // top right
		Vertex(Vec3(1.0f, -1.0f, 0.0f),   Vec2(1.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom right
		Vertex(Vec3(-1.0f, -1.0f, 0.0f),  Vec2(0.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom left
		Vertex(Vec3(-1.0f,  1.0f, 0.0f),  Vec2(0.0f, 1.0f),   Vec3(0, 0, 1))   // top left
	};

	unsigned int plane_inds[] = { 0,1,2, 0,2,3 };

	mSpriteMesh->SetVertexBuffer(plane_verts, 4);
	mSpriteMesh->SetIndexBuffer(plane_inds, 6);
	mSpriteMesh->Create();

	mEmptyTexture = new VulkanTexture;
	mEmptyTexture->Create(2, 2);
	char* empty_texture_data = new char[2 * 2 * 4];
	memset(empty_texture_data, 255, 16);
	mEmptyTexture->AddMipLevel((byte*)empty_texture_data, 16, 2, 2, 0, 0);
	mEmptyTexture->CreateImageView();
	delete[] empty_texture_data;

	//---------------------Samplers------------------
	mMaterialMapsSampler = new VulkanSampler;
	mMaterialMapsSampler->SetWrapModes(SAMPLER_WRAP_REPEAT, SAMPLER_WRAP_REPEAT);
	mMaterialMapsSampler->Create();

	mAttachmentSampler = new VulkanSampler;
	mAttachmentSampler->Create();

	//----------------------Descriptors--------------------------
	mObjectsPool = new VulkanDescriptorPool;
	mMaterialsDescriptorPool = new VulkanDescriptorPool;
	mMaterialsDescriptorPool->SetDescriptorSetsCount(10000);
	mMaterialsDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2000);
	mMaterialsDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16000);
	mMaterialsDescriptorPool->Create();

	//Create base vertex descriptors sets
	for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
		VulkanDescriptorSet* set = new VulkanDescriptorSet(mObjectsPool);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Camera
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT); //Transform
		mVertexDescriptorSets.push_back(set);
	}

	mAnimationsDescriptorSet = new VulkanDescriptorSet(mObjectsPool);
	mAnimationsDescriptorSet->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Animation

	//Create base particles descriptors sets
	mParticlesDescriptorSet = new VulkanDescriptorSet(mObjectsPool);
	mParticlesDescriptorSet->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);

	mDeferredPassSet = new VulkanDescriptorSet(mObjectsPool);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, VK_SHADER_STAGE_FRAGMENT_BIT);
	//shadows
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 7, VK_SHADER_STAGE_FRAGMENT_BIT);
	//depth
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8, VK_SHADER_STAGE_FRAGMENT_BIT);
	//brdf lut
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 9, VK_SHADER_STAGE_FRAGMENT_BIT);
	//specular env map
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10, VK_SHADER_STAGE_FRAGMENT_BIT);
	//irradiance env map
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 11, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Create POOL
	mObjectsPool->Create();
	//Create descriptor sets
	for (uint32 desc_i = 0; desc_i < VERTEX_DESCR_SETS; desc_i++) {
		VulkanDescriptorSet* set = mVertexDescriptorSets[desc_i];
		set->Create();

		set->WriteDescriptorBuffer(0, mCameraShaderBuffer);
		set->WriteDescriptorBuffer(1, mTransformsShaderBuffer, desc_i * 1024 * 64, sizeof(Mat4) * 1024);
	}

	mAnimationsDescriptorSet->Create();
	mAnimationsDescriptorSet->WriteDescriptorBuffer(0, mAnimationTransformsShaderBuffer, 0);
	
	mParticlesDescriptorSet->Create();
	mParticlesDescriptorSet->WriteDescriptorBuffer(0, mParticlesTransformShaderBuffer);
	
	_shadowmapper = new VulkanShadowmapping(
		&mVertexDescriptorSets,
		mAnimationsDescriptorSet,
		mCameraShaderBuffer,
		mSpriteMesh,
		(VulkanTexture*)mGBuffer->GetColorAttachments()[2],
		mAttachmentSampler,
		mEmptyTexture);
	_shadowmapper->SetEntitiesToRender(&_entitiesToRender);

	_brdf_lut = new Vulkan_BRDF_LUT;
	_brdf_lut->Create();

	mDeferredPassSet->Create();
	mDeferredPassSet->WriteDescriptorBuffer(1, mCameraShaderBuffer);
	mDeferredPassSet->WriteDescriptorBuffer(2, _lightsBuffer);
	mDeferredPassSet->WriteDescriptorImage(3, (VulkanTexture*)mGBuffer->GetColorAttachments()[0], mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(4, (VulkanTexture*)mGBuffer->GetColorAttachments()[1], mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(5, (VulkanTexture*)mGBuffer->GetColorAttachments()[2], mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(6, (VulkanTexture*)mGBuffer->GetColorAttachments()[3], mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(7, _shadowmapper->GetOutputTexture(), mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(8, (VulkanTexture*)mGBuffer->GetDepthAttachment(), mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(9, _brdf_lut->GetTextureLut(), mAttachmentSampler);

	//---------------------Command buffers------------------------
	mCmdPool = new VulkanCommandPool;
	mCmdPool->Create(device->GetGraphicsQueueFamilyIndex());

	mGBufferCmdbuf = new VulkanCommandBuffer;
	mGBufferCmdbuf->Create(mCmdPool);

	mLightsCmdbuf = new VulkanCommandBuffer;
	mLightsCmdbuf->Create(mCmdPool);

	//--------------------Pipeline--------------------------------
	VulkanPipelineLayout* p_layout = new VulkanPipelineLayout;
	p_layout->PushDescriptorSet(mDeferredPassSet);
	p_layout->Create();

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	mDeferredPipeline = new VulkanPipeline;
	mDeferredPipeline->Create(deferred_light, mOutputPass, _vertexLayout, p_layout);

	//---Material test ----
	pbr_template = new MaterialTemplate;
	pbr_template->SetName("default_pbr");
	pbr_template->SetShader("PBR");
	pbr_template->AddTexture("Albedo", 1);
	pbr_template->AddTexture("Normal", 2);
	pbr_template->AddTexture("Roughness", 3);
	pbr_template->AddTexture("Metallic", 4);
	pbr_template->AddTexture("Height", 5);
	pbr_template->AddTexture("Occlusion", 6);
	pbr_template->AddTexture("Emission", 7);
	pbr_template->AddParameter("Color", Color(1, 1, 1, 1));
	pbr_template->AddParameter("Roughness factor", 1.f);
	pbr_template->AddParameter("Metallic factor", 1.f);
	MaterialTemplateCache::Get()->AddTemplate(pbr_template);
	CreatePipelineFromMaterialTemplate(pbr_template);

	BlendAttachmentDesc particle_blend_desc;
	particle_blend_desc._blending = true;
	particle_blend_desc._srcColor = BLEND_FACTOR_SRC_ALPHA;
	particle_blend_desc._dstColor = BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;


	particle_template = new MaterialTemplate;
	particle_template->SetName("default_particle");
	particle_template->SetVertexLayout(_vertexLayout);
	particle_template->SetCullMode(CULL_MODE_NONE);
	particle_template->SetBlendingAttachmentDesc(0, particle_blend_desc);
	particle_template->AddTexture("Diffuse", 1);
	particle_template->AddParameter("Color", Color(1,1,1,1));
	particle_template->SetShader("Particle");
	
	MaterialTemplateCache::Get()->AddTemplate(particle_template);
	CreatePipelineFromMaterialTemplate(particle_template);


	skybox_template = new MaterialTemplate;
	skybox_template->SetName("default_skybox");
	skybox_template->SetVertexLayout(_vertexLayout);
	skybox_template->SetDepthTest(false);
	skybox_template->SetCullMode(CULL_MODE_NONE);

	skybox_template->AddTexture("Back", 1);
	skybox_template->AddTexture("Front", 2);
	skybox_template->AddTexture("Left", 3);
	skybox_template->AddTexture("Right", 4);
	skybox_template->AddTexture("Bottom", 5);
	skybox_template->AddTexture("Top", 6);

	skybox_template->AddParameter("Tint color", Color(1, 1, 1, 1));
	skybox_template->AddParameter("Exposure", 1.f);
	skybox_template->SetShader("Skybox");

	MaterialTemplateCache::Get()->AddTemplate(skybox_template);
	CreatePipelineFromMaterialTemplate(skybox_template);
}

void VulkanRenderer::DestroyRenderer() {

}

void VulkanRenderer::StoreWorldObjects() {
	CreateRenderList();

	//send lights to gpu buffer
	uint32 lights_count = this->_lightsources.size();
	_lightsBuffer->WriteData(0, 4, &lights_count);
	for (uint32 light_i = 0; light_i < lights_count; light_i++) {
		LightsourceComponent* light = _lightsources[light_i]->GetComponent<LightsourceComponent>();
		int light_type = light->GetLightType();
		float intensity = light->GetIntensity();
		float range = light->GetRange();
		float spot_angle = light->GetSpotAngle();
		Vec3 pos = _lightsources[light_i]->GetAbsolutePosition();
		Vec3 dir = light->GetDirection();
		Color color = light->GetColor();
		_lightsBuffer->WriteData(16 + light_i * 64, 4, &light_type);
		_lightsBuffer->WriteData(16 + light_i * 64 + 4, 4, &intensity);
		_lightsBuffer->WriteData(16 + light_i * 64 + 8, 4, &range);
		_lightsBuffer->WriteData(16 + light_i * 64 + 12, 4, &spot_angle);
		_lightsBuffer->WriteData(16 + light_i * 64 + 16, 16, &pos);
		_lightsBuffer->WriteData(16 + light_i * 64 + 32, 16, &dir);
		_lightsBuffer->WriteData(16 + light_i * 64 + 48, 16, &color);
	}


	Mat4* transforms = new Mat4[_entitiesToRender.size() * 4];
	Mat4* anim = new Mat4[mAnimationTransformsShaderBuffer->GetSize() / 64];

	_writtenBones = 0;
	_writtenParticleTransforms = 0;

	for (uint32 e_i = 0; e_i < _entitiesToRender.size(); e_i ++) {
		Entity* entity = _entitiesToRender[e_i];
		transforms[e_i * 4] = entity->GetWorldTransform();

		MeshResource* mresource = entity->GetComponent<MeshComponent>()->GetMeshResource();
		AnimatorComponent* anim_comp = entity->GetComponent<AnimatorComponent>();

		for (uint32 bone_i = 0; bone_i < mresource->GetMesh()->GetBones().size(); bone_i++) {
			Bone* bone = &mresource->GetMesh()->GetBones()[bone_i];
			Entity* rootNode = entity->GetRootSkinningEntity();
			Entity* node = nullptr;
			Mat4 rootNodeTransform;

			if (rootNode != nullptr) {
				//if RootNode is specified
				node = rootNode->GetChildEntityWithLabel(bone->GetName());
				//Get root transform
				rootNodeTransform = rootNode->GetLocalTransform().invert();
			}

			if (node != nullptr) {
				//Calculate result matrix
				Mat4 matrix = bone->GetOffsetMatrix().transpose() * node->GetWorldTransform() * rootNodeTransform;
				//Send skinned matrix to skinning uniform buffer
				anim[_writtenBones] = matrix;
				_writtenBones++;
			}
		}

	}

	mTransformsShaderBuffer->WriteData(0, sizeof(Mat4) * 4 * _entitiesToRender.size(), transforms);
	delete[] transforms;

	mAnimationTransformsShaderBuffer->WriteData(0, mAnimationTransformsShaderBuffer->GetSize(), anim);
	delete[] anim;

	for (uint32 e_i = 0; e_i < _entitiesToRender.size(); e_i++) {
		Entity* entity = _entitiesToRender[e_i];
		MaterialComponent* component = entity->GetComponent<MaterialComponent>();
		if (!component)
			continue;
		MaterialResource* resource = (MaterialResource*)entity->GetComponent<MaterialComponent>()->GetResourceReference().GetResource();
		if (!resource)
			continue;
		Material* mat = resource->GetMaterial();
		if(resource->GetState() == RESOURCE_STATE_READY)
			UpdateMaterialDescrSet(mat);
	}

	for (uint32 particle_em_i = 0; particle_em_i < _particleEmitters.size(); particle_em_i++) {
		Entity* entity = _particleEmitters[particle_em_i];
		ParticleEmitterComponent* emitter = entity->GetComponent<ParticleEmitterComponent>();

		MaterialComponent* component = entity->GetComponent<MaterialComponent>();
		if (!component)
			continue;
		MaterialResource* resource = (MaterialResource*)entity->GetComponent<MaterialComponent>()->GetResourceReference().GetResource();
		if (!resource)
			continue;

		if (!emitter->IsSimulating())
			continue;

		Material* mat = resource->GetMaterial();
		UpdateMaterialDescrSet(mat);
		//TEMPORARY
		emitter->StepSimulation();
		//--------------------
		

		Mat4* ParticleTransforms = nullptr;
		emitter->GetParticlesTransforms(&ParticleTransforms, *cam);

		uint32 particles_count = emitter->GetAliveParticlesCount();
		mParticlesTransformShaderBuffer->WriteData(_writtenParticleTransforms * sizeof(Mat4), sizeof(Mat4) * particles_count, ParticleTransforms);

		_writtenParticleTransforms += particles_count;

	}

	//--------------SHADOWMAPPING------------
	_shadowmapper->ResetCasters();
	for (uint32 caster_i = 0; caster_i < _shadowcasters.size(); caster_i++) {
		_shadowmapper->AddEntity(_shadowcasters[caster_i]);
	}
	for (uint32 caster_i = 0; caster_i < _shadowcasters.size(); caster_i++) {
		_shadowmapper->ProcessShadowCaster(caster_i);
	}
	//-----------------------------


	mGBufferCmdbuf->Begin();
	mGBufferPass->CmdBegin(*mGBufferCmdbuf, *mGBuffer);

	_writtenBones = 0;
	_writtenParticleTransforms = 0;

	for (uint32 e_i = 0; e_i < _entitiesToRender.size(); e_i++) {
		Entity* entity = _entitiesToRender[e_i];
		MeshResource* mesh_resource = entity->GetComponent<MeshComponent>()->GetMeshResource();
		MaterialResource* mat_resource = entity->GetComponent<MaterialComponent>()->GetMaterialResource();
		
		if (mat_resource->GetState() != RESOURCE_STATE_READY)
			continue;

		//bind material
		MaterialTemplate* templ = mat_resource->GetMaterial()->GetTemplate();
		VulkanPipeline* pipl = (VulkanPipeline*)templ->GetPipeline();
		Material* mat = mat_resource->GetMaterial();
		VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();

		mGBufferCmdbuf->BindPipeline(*pipl);
		mGBufferCmdbuf->SetViewport(0, 0, mOutputWidth, mOutputHeight);
		VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();
		mGBufferCmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);
		
		if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();
			//Mark mesh resource used in this frame
			mesh_resource->Use();
			//Mark material resource used in this frame
			mat_resource->Use();
			
			uint32 offsets[2] = {0, e_i * UNI_ALIGN };
			uint32 anim_offset = _writtenBones * 64;
			_writtenBones += mesh->GetBones().size();

			int vertexDescriptorID = (e_i * UNI_ALIGN) / 65535;

			mGBufferCmdbuf->BindDescriptorSets(*ppl, 0, 1, mVertexDescriptorSets[vertexDescriptorID], 2, offsets);
			mGBufferCmdbuf->BindDescriptorSets(*ppl, 2, 1, mAnimationsDescriptorSet, 1, &anim_offset);
			mGBufferCmdbuf->BindMesh(*mesh);
			if (mesh->GetIndexCount() > 0)
				mGBufferCmdbuf->DrawIndexed(mesh->GetIndexCount());
			else
				mGBufferCmdbuf->Draw(mesh->GetVerticesCount());
		}

	}

	for (uint32 particle_em_i = 0; particle_em_i < _particleEmitters.size(); particle_em_i++) {
		Entity* entity = _particleEmitters[particle_em_i];
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

		mGBufferCmdbuf->BindPipeline(*pipl);
		mGBufferCmdbuf->SetViewport(0, 0, mOutputWidth, mOutputHeight);
		VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();
		mGBufferCmdbuf->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);

		if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();
			//Mark mesh resource used in this frame
			mesh_resource->Use();
			//Mark material resource used in this frame
			mat_resource->Use();

			uint32 offsets1[2] = { 0, 0 };
			uint32 offset2 = _writtenParticleTransforms * sizeof(Mat4);

			mGBufferCmdbuf->BindDescriptorSets(*ppl, 0, 1, mVertexDescriptorSets[0], 2, offsets1);
			mGBufferCmdbuf->BindDescriptorSets(*ppl, 2, 1, mParticlesDescriptorSet, 1, &offset2);
			mGBufferCmdbuf->BindMesh(*mesh);
			uint32 instances_count = particle_emitter->GetAliveParticlesCount();
			if (mesh->GetIndexCount() > 0)
				mGBufferCmdbuf->DrawIndexed(mesh->GetIndexCount(), instances_count);
			else
				mGBufferCmdbuf->Draw(mesh->GetVerticesCount(), instances_count);

			uint32 particles_count = particle_emitter->GetAliveParticlesCount();
			_writtenParticleTransforms += particles_count;
		}
	}

	mGBufferCmdbuf->EndRenderPass();
	mGBufferCmdbuf->End();


	mLightsCmdbuf->Begin();
	mOutputPass->CmdBegin(*mLightsCmdbuf, *mOutputBuffer);

	DrawSkybox(mLightsCmdbuf);

	mLightsCmdbuf->BindPipeline(*mDeferredPipeline);
	mLightsCmdbuf->SetViewport(0, 0, mOutputWidth, mOutputHeight);
	mLightsCmdbuf->BindDescriptorSets(*mDeferredPipeline->GetPipelineLayout(), 0, 1, mDeferredPassSet);
	mLightsCmdbuf->BindMesh(*mSpriteMesh, 0);
	mLightsCmdbuf->DrawIndexed(6);

	mLightsCmdbuf->EndRenderPass();
	mLightsCmdbuf->End();

}

void VulkanRenderer::DrawSkybox(VulkanCommandBuffer* cmdbuffer) {
	if (mScene) {
		SceneEnvironmentSettings& env_settings = mScene->GetEnvironmentSettings();
		if (env_settings._skybox_material.IsResourceSpecified()) {
			MaterialResource* resource = (MaterialResource*)env_settings._skybox_material.GetResource();
			if (resource) {

				if (resource->IsUnloaded()) {
					resource->Load();
				}

				if (resource->IsReady()) {
					resource->Use();
					Material* mat = resource->GetMaterial();
					UpdateMaterialDescrSet(mat);
					VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();
					MaterialTemplate* templ = resource->GetMaterial()->GetTemplate();
					VulkanPipeline* pipl = (VulkanPipeline*)templ->GetPipeline();
					VulkanPipelineLayout* ppl = pipl->GetPipelineLayout();

					cmdbuffer->BindPipeline(*pipl);
					cmdbuffer->SetViewport(0, 0, mOutputWidth, mOutputHeight);
					uint32 offsets[2] = { 0, 0 };
					cmdbuffer->BindDescriptorSets(*ppl, 0, 1, mVertexDescriptorSets[0], 2, offsets);
					cmdbuffer->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);

					VulkanMesh* cube = (VulkanMesh*)((MeshResource*)GetCubeMesh())->GetMesh();
					cmdbuffer->BindMesh(*cube);
					cmdbuffer->Draw(cube->GetVerticesCount());
				}
			}
		}
	}
}

void VulkanRenderer::DrawScene(VSGE::Camera* cam) {
	//TEMPORARY
	this->cam = cam;
	_shadowmapper->SetCamera(cam);
	_shadowmapper->SetScene(mScene);
	//---------------------

	cam->UpdateMatrices();
	mCameraShaderBuffer->WriteData(0, sizeof(Mat4), (void*)&cam->GetProjectionViewMatrix());

	Mat4 skybox_viewproj = RemoveTranslationFromViewMat(cam->GetViewMatrix()) * cam->GetProjectionMatrix();

	mCameraShaderBuffer->WriteData(sizeof(Mat4), sizeof(Mat4), &skybox_viewproj);
	mCameraShaderBuffer->WriteData(sizeof(Mat4) * 2, sizeof(Vec3), (void*)&cam->GetPosition());

	for (uint32 camera_i = 0; camera_i < _cameras.size(); camera_i++) {
		Camera* camera = _cameras[camera_i]->GetComponent<Camera>();
		camera->UpdateMatrices();
		mCameraShaderBuffer->WriteData(
			camera_i * UNI_ALIGN,
			sizeof(Mat4),
			(void*)&camera->GetProjectionViewMatrix());
	}

	StoreWorldObjects();

	for (uint32 caster_i = 0; caster_i < _shadowcasters.size(); caster_i++) {
		VulkanSemaphore* shadowbegin = nullptr;
		VulkanSemaphore* shadowend = nullptr;

		if (caster_i == 0)
			shadowbegin = mBeginSemaphore;
		if (caster_i == _shadowcasters.size() - 1)
			shadowend = mShadowmappingEndSemaphore;

		_shadowmapper->ExecuteShadowCaster(caster_i, shadowbegin, shadowend);
	}
	VulkanSemaphore* begin = mShadowmappingEndSemaphore;
	if (_shadowcasters.size() == 0)
		begin = mBeginSemaphore;

	_shadowmapper->RenderShadows(begin, mShadowprocessingEndSemaphore);

	VulkanGraphicsSubmit(*mGBufferCmdbuf, *mShadowprocessingEndSemaphore, *mGBufferSemaphore);

	VulkanGraphicsSubmit(*mLightsCmdbuf, *mGBufferSemaphore, *mEndSemaphore);
}

void VulkanRenderer::ResizeOutput(uint32 width, uint32 height) {
	mOutputWidth = width;
	mOutputHeight = height;

	mGBuffer->SetSize(width, height);
	mOutputBuffer->SetSize(width, height);

	mGBufferPass->SetClearSize(width, height);
	mOutputPass->SetClearSize(width, height);
}