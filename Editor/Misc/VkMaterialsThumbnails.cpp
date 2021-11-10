#include "VkMaterialsThumbnails.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <algorithm>

using namespace VSGE;

VkMaterialsThumbnails* VkMaterialsThumbnails::_this = nullptr;

VkMaterialThumbnail::VkMaterialThumbnail(VulkanSampler* sampler) {
    texture = new VulkanTexture;
    texture->Create(THUMBNAIL_TEXTURE_SIZE, THUMBNAIL_TEXTURE_SIZE);
    texture->ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    deleted = false;

    imtexture = ImGui_ImplVulkan_AddTexture(sampler->GetSampler(), texture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkMaterialThumbnail::~VkMaterialThumbnail() {
    SAFE_RELEASE(texture);
}

VkMaterialsThumbnails::VkMaterialsThumbnails() {
    _gbuffer = nullptr;
    _light = nullptr;
    _this = this;
}
VkMaterialsThumbnails::~VkMaterialsThumbnails() {
    Destroy();
}

void VkMaterialsThumbnails::Create() {
    VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

    _thumb_scene = new Scene;
    _thumb_scene->NewScene();
    _thumb_entity = _thumb_scene->AddNewEntity("thumb_sphere");
    MeshComponent* mesh_component = _thumb_entity->AddComponent<MeshComponent>();
    mesh_component->SetMeshName("Sphere");
    _thumb_entity->AddComponent<MaterialComponent>();

    _entities_to_render.push_back(_thumb_entity);

    _transform_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
    _transform_buffer->Create(256);
    Mat4 transform = Mat4(1.f);
    _transform_buffer->WriteData(0, 64, &transform);

    _storage_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE); 
    _storage_buffer->Create(256);

    _lights_buffer = new LightsBuffer;
    _lights_buffer->SetMaxLightsCount(1);
    _lights_buffer->Create();
    _lights_buffer->SetLight(0, 0, 10, 0, 0, Vec3(0.f, 0.f, 0.f), Vec3(0.4f, 0.3f, -0.7f), Color());
    _lights_buffer->SetLightsCount(1);
    _lights_buffer->SetAmbientColor(Color());
    _lights_buffer->UpdateGpuBuffer();

    _camera = new Camera;
    _camera->SetPosition(Vec3(0, 0, -1.5f));
    _camera->SetFront(Vec3(0, 0, 1));
    _camera->SetUp(Vec3(0, 1, 0));
    _camera->SetAspectRatio(1.f);
    _camera->SetNearPlane(0.1f);
    _camera->SetFarPlane(1000.f);
    _camera->SetFOV(90.f);
    _camera->UpdateMatrices();
    VulkanRenderer::Get()->GetCamerasBuffer()->SetCamera(99, _camera);

    _gbuffer = new VulkanGBufferRenderer;
    _gbuffer->CreateFramebuffer();
    _gbuffer->CreateDescriptorSets();
    _gbuffer->SetEntitiesToRender(_entities_to_render, _particles_to_render);
    _gbuffer->SetCameraIndex(99);
    _gbuffer->Resize(THUMBNAIL_TEXTURE_SIZE, THUMBNAIL_TEXTURE_SIZE);
    _gbuffer->SetBuffers(_transform_buffer, _storage_buffer, _storage_buffer);
    _gbuffer->GetRenderPass()->SetClearColor(0, Color(0, 0, 0, 0));

    _light = new VulkanDeferredLight;
    _light->SetOutputFormat8();
    _light->CreateFramebuffer();
    _light->CreateDescriptorSet();
    _light->CreatePipeline();
    _light->SetLightsBuffer((VulkanBuffer*)_lights_buffer->GetLightsGpuBuffer());
    _light->SetGBuffer(_gbuffer);
    _light->Resize(THUMBNAIL_TEXTURE_SIZE, THUMBNAIL_TEXTURE_SIZE);
    _light->SetCameraIndex(99);
    _light->GetRenderPass()->SetClearColor(0, Color(0, 0, 0, 0));

    _cmdpool = new VulkanCommandPool;
    _cmdpool->Create(device->GetGraphicsQueueFamilyIndex());

    _cmdbuf = new VulkanCommandBuffer;
    _cmdbuf->Create(_cmdpool);

    _begin_semaphore = new VulkanSemaphore();
    _begin_semaphore->Create();

    _sampler = new VulkanSampler();
    _sampler->Create();
}

void VkMaterialsThumbnails::Destroy() {
    SAFE_RELEASE(_light)
    SAFE_RELEASE(_gbuffer)
}

void VkMaterialsThumbnails::RecreateAll() {

}
void VkMaterialsThumbnails::CreateThumbnail(const std::string& material_name) {
    //find queued with this name
    bool add = true;
    for (auto& name : _queued) {
        if (name == material_name) {
            add = false;
        }
    }
    if(add)
        _queued.push_back(material_name);
}
VkMaterialThumbnail* VkMaterialsThumbnails::GetMaterialThumbnail(const std::string& material_name) {
    for (auto& thumb : _thumbnails) {
        if (thumb->material_name == material_name)
            return thumb;
    }
    return nullptr;
}
ImTextureID VkMaterialsThumbnails::GetMaterialThumbnailTexture(const std::string& material_name) {
    VkMaterialThumbnail* thumb = GetMaterialThumbnail(material_name);
    if (thumb)
        return thumb->imtexture;
    return nullptr;
}

VkMaterialThumbnail* VkMaterialsThumbnails::GetPlace(const std::string& name) {
    //check for empty space
    for (auto& thumb : _thumbnails) {
        if (thumb->deleted)
            return thumb;
    }
    //check for thumb with this name
    VkMaterialThumbnail* thumb = GetMaterialThumbnail(name);
    if (thumb)
        return thumb;

    //create new
    VkMaterialThumbnail* new_thumb = new VkMaterialThumbnail(_sampler);
    _thumbnails.push_back(new_thumb);
    return _thumbnails[_thumbnails.size() - 1];
}

void VkMaterialsThumbnails::RecordCmdBuffer() {

    _cmdbuf->Begin();
    if (_queued.size() > 0) {
        MaterialComponent* material_component = _thumb_entity->GetComponent<MaterialComponent>();
        material_component->SetMaterialName(_queued[0]);

        MaterialResource* resource = (MaterialResource*)material_component->GetResourceReference().GetResource();
        Material* mat = resource->GetMaterial();
        if (resource->GetState() == RESOURCE_STATE_READY)
            VulkanRenderer::Get()->UpdateMaterialDescrSet(mat);

        _gbuffer->RecordCmdBuffer(_cmdbuf);
        _light->RecordCmdbuf(_cmdbuf);

        VkMaterialThumbnail* thumb = GetPlace(_queued[0]);
        thumb->material_name = _queued[0];
        VulkanTexture* src = (VulkanTexture*)_light->GetFramebuffer()->GetColorAttachments()[0];
        VulkanTexture* dst = thumb->texture;

        src->CmdCopyTexture(_cmdbuf, dst, 0, 1);

        //shrink array to left
        for (uint32 q_i = 1; q_i < _queued.size(); q_i++) {
            _queued[q_i - 1] = _queued[q_i];
        }
        _queued.pop_back();
    }
    _cmdbuf->End();
    
}
void VkMaterialsThumbnails::CmdExecute(VulkanSemaphore* end) {
    VulkanGraphicsSubmit(*_cmdbuf, *_begin_semaphore, *end);
}
VulkanSemaphore* VkMaterialsThumbnails::GetBeginSemaphore() {
    return _begin_semaphore;
}