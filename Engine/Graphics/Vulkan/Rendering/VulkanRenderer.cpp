#include "VulkanRenderer.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanShader.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>

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
	deferred_light->AddShaderFromFile("deferred.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(deferred_light, "Deferred");

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();
	//--------------------Framebuffers----------------
	mGBufferPass = new VulkanRenderPass;
	mGBufferPass->SetClearSize(mOutputWidth, mOutputHeight);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA16F);
	mGBufferPass->PushColorAttachment(FORMAT_RGBA16F);
	mGBufferPass->PushDepthAttachment();
	mGBufferPass->Create();

	mGBuffer = new VulkanFramebuffer;
	mGBuffer->SetSize(mOutputWidth, mOutputHeight);
	mGBuffer->AddAttachment(FORMAT_RGBA); //Color
	mGBuffer->AddAttachment(FORMAT_RGBA16F); //Normal
	mGBuffer->AddAttachment(FORMAT_RGBA16F); //Position
	mGBuffer->AddDepth();
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

	//---------------------Buffers--------------------------------
	mCameraShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mCameraShaderBuffer->Create(UNI_ALIGN);

	mTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mTransformsShaderBuffer->Create(UNI_ALIGN * MAX_OBJECTS_RENDER);

	mAnimationTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mAnimationTransformsShaderBuffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

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

	//---------------------Samplers------------------
	mMaterialMapsSampler = new VulkanSampler;
	mMaterialMapsSampler->Create();

	mAttachmentSampler = new VulkanSampler;
	mAttachmentSampler->Create();

	//----------------------Descriptors--------------------------
	mObjectsPool = new VulkanDescriptorPool;
	mMaterialsDescriptorPool = new VulkanDescriptorPool;
	mMaterialsDescriptorPool->SetDescriptorSetsCount(2000);
	mMaterialsDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2000);
	mMaterialsDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000);
	mMaterialsDescriptorPool->Create();

	for (uint32 desc_i = 0; desc_i < MAX_OBJECTS_RENDER; desc_i++) {
		VulkanDescriptorSet* set = new VulkanDescriptorSet(mObjectsPool);
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_VERTEX_BIT); //Camera
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT); //Transform
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, VK_SHADER_STAGE_VERTEX_BIT); //Animation

		mVertexDescriptorSets.push_back(set);
	}

	mDeferredPassSet = new VulkanDescriptorSet(mObjectsPool);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
	mDeferredPassSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Create POOL
	mObjectsPool->Create();
	//Create descriptor sets
	for (uint32 desc_i = 0; desc_i < MAX_OBJECTS_RENDER; desc_i++) {
		VulkanDescriptorSet* set = mVertexDescriptorSets[desc_i];
		set->Create();

		set->WriteDescriptorBuffer(0, mCameraShaderBuffer);
		set->WriteDescriptorBuffer(1, mTransformsShaderBuffer, desc_i * UNI_ALIGN, sizeof(Mat4));
		set->WriteDescriptorBuffer(2, mAnimationTransformsShaderBuffer, 0, 64 * 201);
	}

	mDeferredPassSet->Create();
	mDeferredPassSet->WriteDescriptorImage(3, (VulkanTexture*)mGBuffer->GetColorAttachments()[0], mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(4, (VulkanTexture*)mGBuffer->GetColorAttachments()[1], mAttachmentSampler);
	mDeferredPassSet->WriteDescriptorImage(5, (VulkanTexture*)mGBuffer->GetColorAttachments()[2], mAttachmentSampler);

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
	_vertexLayout.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

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
	pbr_template->AddTexture("AO", 6);
	MaterialTemplateCache::Get()->AddTemplate(pbr_template);
	CreatePipelineFromMaterialTemplate(pbr_template);
}

void VulkanRenderer::DestroyRenderer() {

}

void VulkanRenderer::StoreWorldObjects() {
	CreateRenderList();
	Mat4* transforms = new Mat4[_entitiesToRender.size() * 4];
	Mat4* anim = new Mat4[1000];
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
				int len = 0;
				node = rootNode->GetChildEntityWithLabelStartsWith(bone->GetName(), len);
				//Get root transform
				rootNodeTransform = rootNode->GetLocalTransform().invert();
			}

			if (node != nullptr) {
				//Calculate result matrix
				Mat4 matrix = (rootNodeTransform * node->GetWorldTransform() * bone->GetOffsetMatrix());
				//Send skinned matrix to skinning uniform buffer
				anim[bone_i] = matrix.transpose();
			}
		}
	}

	mTransformsShaderBuffer->WriteData(0, sizeof(Mat4) * 4 * _entitiesToRender.size(), transforms);
	delete[] transforms;

	mAnimationTransformsShaderBuffer->WriteData(0, sizeof(Mat4) * 1000, anim);
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
		

		CreateVulkanMaterial(mat);

		VulkanMaterial* vmat = static_cast<VulkanMaterial*>(mat->GetDescriptors());

		if (mat->_texturesDirty) {
			for (MaterialTexture& tex : mat->GetTextures()) {
				TextureResource* texture_res = static_cast<TextureResource*>(tex._resource.GetResource());
				if (texture_res == nullptr)
					continue;

				if (texture_res->GetState() == RESOURCE_STATE_UNLOADED) {
					texture_res->Load();
				}

				if (texture_res->GetState() == RESOURCE_STATE_LOADED) {
					texture_res->PostLoad();
					texture_res->SetState(RESOURCE_STATE_READY);
				}
				if (texture_res->GetState() == RESOURCE_STATE_READY) {
					vmat->_fragmentDescriptorSet->WriteDescriptorImage(tex._binding, (VulkanTexture*)texture_res->GetTexture(), this->mMaterialMapsSampler);
					mat->_texturesDirty = false;
				}
			}
		}

		if (mat->_paramsDirty) {
			char* buffer = nullptr;
			uint32 size = mat->CopyParamsToBuffer(&buffer);

			vmat->_paramsBuffer->WriteData(0, size, buffer);
			delete[] buffer;
			mat->_paramsDirty = false;
		}
	}

	mGBufferCmdbuf->Begin();
	mGBufferPass->CmdBegin(*mGBufferCmdbuf, *mGBuffer);


	for (uint32 e_i = 0; e_i < _entitiesToRender.size(); e_i++) {
		Entity* entity = _entitiesToRender[e_i];
		MeshResource* mresource = entity->GetComponent<MeshComponent>()->GetMeshResource();
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
		
		if (mresource->GetState() == RESOURCE_STATE_LOADED) {
			VulkanMesh* mesh = (VulkanMesh*)mresource->GetMesh();
			
			
			mGBufferCmdbuf->BindDescriptorSets(*ppl, 0, 1, mVertexDescriptorSets[e_i]);
			mGBufferCmdbuf->BindMesh(*mesh);
			mGBufferCmdbuf->DrawIndexed(mesh->GetIndexCount());
		}
	}

	mGBufferCmdbuf->EndRenderPass();
	mGBufferCmdbuf->End();


	mLightsCmdbuf->Begin();
	mOutputPass->CmdBegin(*mLightsCmdbuf, *mOutputBuffer);

	mLightsCmdbuf->BindPipeline(*mDeferredPipeline);
	mLightsCmdbuf->SetViewport(0, 0, mOutputWidth, mOutputHeight);
	mLightsCmdbuf->BindDescriptorSets(*mDeferredPipeline->GetPipelineLayout(), 0, 1, mDeferredPassSet);
	mLightsCmdbuf->BindMesh(*mSpriteMesh, 0);
	mLightsCmdbuf->DrawIndexed(6);

	mLightsCmdbuf->EndRenderPass();
	mLightsCmdbuf->End();

}

void VulkanRenderer::DrawScene(VSGE::Camera* cam) {
	cam->UpdateMatrices();
	mCameraShaderBuffer->WriteData(0, sizeof(Mat4), (void*)&cam->GetProjectionViewMatrix());

	StoreWorldObjects();

	VulkanGraphicsSubmit(*mGBufferCmdbuf, *mBeginSemaphore, *mGBufferSemaphore);

	VulkanGraphicsSubmit(*mLightsCmdbuf, *mGBufferSemaphore, *mEndSemaphore);
}

void VulkanRenderer::BindMaterial(Material* mat) {

}

VulkanPipeline* VulkanRenderer::CreatePipelineFromMaterialTemplate(MaterialTemplate* mat_template) {
	VulkanPipelineLayout* p_layout = new VulkanPipelineLayout;
	//Add common vertex descriptor
	p_layout->PushDescriptorSet(mVertexDescriptorSets[0]);
	//Add materials descriptor
	VulkanDescriptorSet* materialsDescrSet = CreateDescriptorSetFromMaterialTemplate(mat_template);
	p_layout->PushDescriptorSet(materialsDescrSet);
	//Create pipeline layout
	p_layout->Create();

	//Create pipeline
	VulkanPipeline* pipeline = new VulkanPipeline;
	pipeline->SetDepthTest(true);
	pipeline->SetCullMode(CULL_MODE_FRONT);
	pipeline->Create((VulkanShader*)mat_template->GetShader(), mGBufferPass, mat_template->GetLayout(), p_layout);
	//Store pipeline pointer in material template
	mat_template->SetPipeline(pipeline);

	return pipeline;
}

VulkanMaterial* VulkanRenderer::CreateVulkanMaterial(Material* material) {
	if (!material->GetTemplate())
		return nullptr;

	if (material->GetDescriptors() == nullptr) {
		VulkanMaterial* mat = new VulkanMaterial;
		//Create params buffer
		mat->_paramsBuffer->Create(1024);
		//Create descriptor from material template
		mat->_fragmentDescriptorSet = CreateDescriptorSetFromMaterialTemplate(material->GetTemplate());
		//Add buffer to descriptor
		mat->_fragmentDescriptorSet->WriteDescriptorBuffer(0, mat->_paramsBuffer, 0, 1024);
		//Store descriptor pointer in material
		material->SetDescriptors(mat);

		return mat;
	}

	return nullptr;
}

VulkanDescriptorSet* VulkanRenderer::CreateDescriptorSetFromMaterialTemplate(MaterialTemplate* mat_template) {
	VulkanDescriptorSet* set = new VulkanDescriptorSet;
	set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	for (MaterialTexture& texture : mat_template->GetTextures())
	{
		set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture._binding, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	set->SetDescriptorPool(mMaterialsDescriptorPool);
	set->Create();

	return set;
}