#include "VulkanRenderer.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanShader.hpp"
#include <Core/Time.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Resources/DefaultResources.hpp>
#include <Math/MatrixCamera.hpp>
#include <Graphics/MaterialTemplateCache.hpp>
#include "VulkanRenderingGenerics.hpp"

using namespace VSGE;

VulkanRenderer* VulkanRenderer::_this = nullptr;

VulkanRenderer::VulkanRenderer() : IRenderer() {
	_this = this;
	SetupRenderer();
}

VulkanRenderer::~VulkanRenderer() {
	DestroyRenderer();
}

void VulkanRenderer::SetupRenderer() {
	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();
	//-----------------------Prepare semaphores-------------------
	mBeginSemaphore = new VulkanSemaphore;
	mBeginSemaphore->Create();

	mEndSemaphore = new VulkanSemaphore;
	mEndSemaphore->Create();
	
	//---------------------Buffers--------------------------------
	_cameras_buffer = new VulkanCamerasBuffer;

	mTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	mTransformsShaderBuffer->Create(MAX_OBJECTS_RENDER * UNI_ALIGN);

	mAnimationTransformsShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	mAnimationTransformsShaderBuffer->Create(sizeof(Mat4) * MAX_ANIMATION_MATRICES);

	mParticlesTransformShaderBuffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	mParticlesTransformShaderBuffer->Create(sizeof(Mat4) * MAX_PARTICLES_MATRICES);

	_lights_buffer = new LightsBuffer;
	_lights_buffer->Create();

	VulkanRenderingGenerics::Get()->Init();

	//----------------------Descriptors--------------------------
	mMaterialsDescriptorPool = new VulkanDescriptorPool;
	mMaterialsDescriptorPool->SetDescriptorSetsCount(10000);
	mMaterialsDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2000);
	mMaterialsDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16000);
	mMaterialsDescriptorPool->Create();

	_ui_renderer = new VulkanUiRenderer;
	_ui_renderer->Create();
	_ui_renderer->ResizeOutput(mOutputWidth, mOutputHeight);

	_ibl_map = new VulkanIBL;
	_ibl_map->SetInputData(
		_entitiesToRender,
		_particleEmitters,
		mTransformsShaderBuffer,
		mAnimationTransformsShaderBuffer,
		mParticlesTransformShaderBuffer,
		(VulkanBuffer*)_lights_buffer->GetLightsGpuBuffer());
	_ibl_map->Create();
	_ibl_map->SetSpmapIrmapAlternately(true);
	//_ibl_map->SetEnabled(false);

	//---------------------Command buffers------------------------
	_cmdpool = new VulkanCommandPool;
	_cmdpool->Create(device->GetGenericQueueFamilyIndex());

	_render_targets_cmdbuf = new VulkanCommandBuffer;
	_render_targets_cmdbuf->Create(_cmdpool);

	_main_render_target = new VulkanRenderTarget;
	_main_render_target->SetCameraIndex(0);
	_main_render_target->SetEntitiesToRender(_entitiesToRender, _particleEmitters);
	_main_render_target->SetBuffers(
		mTransformsShaderBuffer,
		mAnimationTransformsShaderBuffer,
		mParticlesTransformShaderBuffer);
	
	_main_render_target->SetIBL(_ibl_map);

	_render_targets.resize(1);
	_render_targets[0] = _main_render_target;

	VulkanGBufferRenderer* gbuffer = _main_render_target->GetGBufferRenderer();

	_shadowmapper = new VulkanShadowmapping(
		&gbuffer->GetVertexDescriptorSets(),
		gbuffer->GetAnimationsDescriptorSet(),
		_cameras_buffer->GetCamerasBuffer(),
		gbuffer->GetPositionAttachment());
	_shadowmapper->SetEntitiesToRender(&_entitiesToRender);

	_main_render_target->SetShadowmapper(_shadowmapper);

	_final_pass = new VulkanFinalPass;

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	
	MaterialTemplate* pbr_template = new MaterialTemplate;
	pbr_template->SetName("default_pbr");
	pbr_template->SetShader("PBR");
	pbr_template->AddTexture("Albedo", 1);
	pbr_template->AddTexture("Normal", 2);
	pbr_template->AddTexture("Roughness", 3);
	pbr_template->AddTexture("Metallic", 4);
	pbr_template->AddTexture("Height", 5);
	pbr_template->AddTexture("Occlusion", 6);
	pbr_template->AddParameter("Color", Color(1, 1, 1, 1));
	pbr_template->AddParameter("Roughness factor", 1.f);
	pbr_template->AddParameter("Metallic factor", 0.5f);
	pbr_template->AddParameter("Height factor", 1.f);
	MaterialTemplateCache::Get()->AddTemplate(pbr_template);
	CreatePipelineFromMaterialTemplate(pbr_template);

	BlendAttachmentDesc particle_blend_desc;
	particle_blend_desc._blending = true;
	particle_blend_desc._srcColor = BLEND_FACTOR_SRC_ALPHA;
	particle_blend_desc._dstColor = BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;


	MaterialTemplate* particle_template = new MaterialTemplate;
	particle_template->SetName("default_particle");
	particle_template->SetDepthTest(false);
	particle_template->SetRenderStage(RENDER_STAGE_POST);
	particle_template->SetVertexLayout(_vertexLayout);
	particle_template->SetCullMode(CULL_MODE_NONE);
	particle_template->SetBlendingAttachmentDesc(0, particle_blend_desc);
	particle_template->AddTexture("Diffuse", 1);
	particle_template->AddParameter("Color", Color(1,1,1,1));
	particle_template->SetShader("Particle");
	
	MaterialTemplateCache::Get()->AddTemplate(particle_template);
	CreatePipelineFromMaterialTemplate(particle_template);


	MaterialTemplate* vegetable_template = new MaterialTemplate;
	vegetable_template->SetRenderStage(RENDER_STAGE_POST);
	vegetable_template->SetName("default_vegetable");
	vegetable_template->SetShader("Vegetable");
	vegetable_template->AddTexture("Albedo", 1);
	vegetable_template->AddTexture("Normal", 2);
	vegetable_template->AddTexture("Roughness", 3);
	vegetable_template->AddTexture("Metallic", 4);
	vegetable_template->AddTexture("Occlusion", 5);
	vegetable_template->AddTexture("Emission", 6);
	vegetable_template->AddParameter("Color", Color(1, 1, 1, 1));
	vegetable_template->AddParameter("Roughness factor", 1.f);
	vegetable_template->AddParameter("Metallic factor", 0.5f);
	vegetable_template->AddParameter("Height factor", 1.f);
	vegetable_template->SetBlendingAttachmentDesc(0, particle_blend_desc);
	MaterialTemplateCache::Get()->AddTemplate(vegetable_template);
	CreatePipelineFromMaterialTemplate(vegetable_template);


	MaterialTemplate* skybox_template = new MaterialTemplate;
	skybox_template->SetName("default_skybox");
	skybox_template->SetRenderStage(RENDER_STAGE_POST);
	skybox_template->SetVertexLayout(_vertexLayout);
	skybox_template->SetDepthTest(false);
	skybox_template->SetCullMode(CULL_MODE_FRONT);

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
	delete _shadowmapper;

	for (auto& render_target : _render_targets) {
		SAFE_RELEASE(render_target);
	}
	_render_targets.clear();

	SAFE_RELEASE(_ibl_map)
	SAFE_RELEASE(_ui_renderer)
}

void VulkanRenderer::StoreWorldObjects(Camera* cam) {
	if (mScene) {
		SceneEnvironmentSettings& env_settings = mScene->GetEnvironmentSettings();
		//send lights to gpu buffer
		_lights_buffer->SetLightsCount((uint32)this->_lightsources.size());
		_lights_buffer->SetAmbientColor(env_settings.GetAmbientColor());
		for (uint32 light_i = 0; light_i < _lightsources.size(); light_i++) {
			LightsourceComponent* light = _lightsources[light_i]->GetComponent<LightsourceComponent>();
			_lights_buffer->SetLight(light_i, light);
		}
		_lights_buffer->UpdateGpuBuffer();
	}

	Mat4* transforms = new Mat4[_entitiesToRender.size() * 4];
	Mat4* anim = new Mat4[mAnimationTransformsShaderBuffer->GetSize() / 64];

	_writtenBones = 0;
	_writtenParticleTransforms = 0;

	for (uint32 e_i = 0; e_i < _entitiesToRender.size(); e_i ++) {
		Entity* entity = _entitiesToRender[e_i];
		transforms[e_i * 4] = entity->GetWorldTransform();

		MeshComponent* mesh_component = entity->GetComponent<MeshComponent>();
		if (!mesh_component)
			continue;

		MeshResource* mresource = mesh_component->GetMeshResource();
		if (!mresource)
			continue;
		if (mresource->GetMesh()->GetBones().size() > 0) {
			Entity* rootNode = entity->GetRootSkinningEntity();
			Mat4 rootNodeTransform;
			if (rootNode != nullptr) {
				//Get root transform
				rootNodeTransform = rootNode->GetLocalTransform().invert();
				for (uint32 bone_i = 0; bone_i < mresource->GetMesh()->GetBones().size(); bone_i++) {
					//Берем указатель на очередную кость
					const Bone* bone = &mresource->GetMesh()->GetBones()[bone_i];
					//Находим дочерний объект с именем кости
					Entity* node = rootNode->GetChildEntityWithLabel(bone->GetName());
					//Проверяем, найден ли объект
					if (node != nullptr) {
						//Объект найден
						//Calculate result matrix
						Mat4 matrix = bone->GetOffsetMatrix() * node->GetWorldTransform() * rootNodeTransform;
						//Send skinned matrix to skinning uniform buffer
						anim[_writtenBones] = matrix;
						_writtenBones++;
					}
				}
			}
		}
	}

	mTransformsShaderBuffer->WriteData(0, sizeof(Mat4) * 4 * (uint32)_entitiesToRender.size(), transforms);
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
	//-----------------------------

	//Обновляем данные буферов интерфейса
	_ui_renderer->FillBuffers();
}

void VulkanRenderer::FillCommandBuffers()
{
	//Заполняем основной буфер команд отрисовки мира (тени + render targets)
	_render_targets_cmdbuf->Begin();
	_shadowmapper->ProcessShadowCasters(_render_targets_cmdbuf);
	for (auto& render_target : _render_targets) {
		render_target->RecordCommandBuffers(_render_targets_cmdbuf);
	}

	//Заполняем буфер команд отрисовки интерфейса
	_ui_renderer->RecordCommandBuffer(_render_targets_cmdbuf);

	//Заполняем буфер команд совмещения мира и интерфейса
	_final_pass->RecordCmdbuffer(_render_targets_cmdbuf);

	_render_targets_cmdbuf->End();

	_ibl_map->RecordCmdBufs();
}

void VulkanRenderer::DrawScene(VSGE::Camera* cam) 
{
	//TEMPORARY
	//if(mScene)
	//	mScene->UpdateSceneTree();
	_ibl_map->SetScene(mScene);
	//---------------------
	CreateRenderList();
	_cameras_buffer->SetAllCamerasNull();
	for (uint32 camera_i = 0; camera_i < _cameras.size(); camera_i++) {
		Camera* camera = _cameras[camera_i]->GetComponent<Camera>();
		_cameras_buffer->SetCamera(camera_i, camera);
	}

	Camera* main_camera = nullptr;

	if (cam) {
		main_camera = cam;
		_shadowmapper->SetCamera(cam);
		_cameras_buffer->SetEnvmapCameras(cam->GetPosition(), 100.f);
		_cameras_buffer->SetCamera(0, cam);
	}
	else if(_cameras.size() > 0) {
		Camera* camera = _cameras[0]->GetComponent<Camera>();
		if (camera) {
			main_camera = camera;
			_shadowmapper->SetCamera(camera);
			_cameras_buffer->SetEnvmapCameras(camera->GetPosition(), 100.f);
		}
	}

	_cameras_buffer->UpdateGpuBuffer();

	while (_render_targets.size() < _cameras.size()) {
		VulkanRenderTarget* render_target = new VulkanRenderTarget;
		_render_targets.push_back(render_target);
	}

	for (uint32 camera_i = 0; camera_i < _cameras.size(); camera_i++) {
		Camera* camera = _cameras_buffer->GetCameraByIndex(camera_i);
		VulkanRenderTarget* render_target = _render_targets[camera_i];
		render_target->SetCameraIndex(camera_i);
		render_target->SetEntitiesToRender(_entitiesToRender, _particleEmitters);
		render_target->SetBuffers(
			mTransformsShaderBuffer,
			mAnimationTransformsShaderBuffer,
			mParticlesTransformShaderBuffer);
		TextureResource* resource = camera->GetTargetResource();
		
		//Если камере назначен Render Target
		if (resource) {
			if (resource->IsUnloaded()) {
				resource->Load();
				continue;
			}
			VulkanTexture* target_texture = (VulkanTexture*)resource->GetTexture();
			render_target->SetOutput(target_texture);
			render_target->ResizeOutput(target_texture->GetWidth(), target_texture->GetHeight());
		}
		else {
			render_target->SetOutput(nullptr);
		}

		camera->SetAspectRatio((float)render_target->GetWidth() / render_target->GetHeight());
	}

	//Обновить состояния объектов, заполнить массивы трансформаций, обновить дескрипторы
	StoreWorldObjects(main_camera);

	//Заполнить буферы команд
	FillCommandBuffers();

	VulkanGraphicsSubmit(*_render_targets_cmdbuf, *mBeginSemaphore, *_ibl_map->GetBeginSemaphore());

	_ibl_map->Execute(mEndSemaphore);
}

void VulkanRenderer::ResizeOutput(uint32 width, uint32 height) {
	mOutputWidth = width;
	mOutputHeight = height;

	//Изменить размер главного Render target
	_main_render_target->ResizeOutput(width, height);

	//Изменить размер выходного изображения отрисовщика интерфейса
	_ui_renderer->ResizeOutput(width, height);

	mOutput = _main_render_target->GetGammaCorrectedOutput();

	GetFinalPass()->Resize(width, height);
	GetFinalPass()->SetAttachments(_main_render_target->GetGammaCorrectedOutput(), GetUiAttachment());
}

VulkanSemaphore* VulkanRenderer::GetBeginSemaphore() {
	return mBeginSemaphore;
}

void VulkanRenderer::SetBeginSemaphore(VulkanSemaphore* semaphore) {
	delete mBeginSemaphore;
	mBeginSemaphore = semaphore;
}

VulkanSemaphore* VulkanRenderer::GetEndSemaphore() {
	return mEndSemaphore;
}

VulkanCamerasBuffer* VulkanRenderer::GetCamerasBuffer() {
	return _cameras_buffer;
}

VulkanUiRenderer* VulkanRenderer::GetUiRenderer() {
	return _ui_renderer;
}

VulkanFinalPass* VulkanRenderer::GetFinalPass() {
	return _final_pass;
}

VulkanRenderPass* VulkanRenderer::GetMaterialsRenderPass() {
	return _main_render_target->GetGBufferRenderer()->GetRenderPass();
}

VulkanRenderTarget* VulkanRenderer::GetMainRenderTarget() {
	return _main_render_target;
}

VulkanTexture* VulkanRenderer::GetUiAttachment() {
	return _ui_renderer->GetOutputTexture();
}

LightsBuffer* VulkanRenderer::GetLightsBuffer() {
	return _lights_buffer;
}

VulkanCommandPool* VulkanRenderer::GetCommandPool() {
	return _cmdpool;
}