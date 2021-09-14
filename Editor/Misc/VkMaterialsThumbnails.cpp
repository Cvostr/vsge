#include "VkMaterialsThumbnails.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>

using namespace VSGE;

VkMaterialsThumbnails* VkMaterialsThumbnails::_this = nullptr;

VkMaterialsThumbnails::VkMaterialsThumbnails() {
    _gbuffer = nullptr;
    _light = nullptr;
}
VkMaterialsThumbnails::~VkMaterialsThumbnails() {
    Destroy();
}

void VkMaterialsThumbnails::Create() {
    VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

    _thumb_scene = new Scene;
    VSGE::Entity* entity = _thumb_scene->AddNewEntity("thumb_sphere");
    MeshComponent* mesh_component = entity->AddComponent<MeshComponent>();
    MaterialComponent* material_component = entity->AddComponent<MaterialComponent>();

    _entities_to_render.push_back(entity);

    _transform_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
    _transform_buffer->Create(256);
    Mat4 transform = Mat4(1.f);
    _transform_buffer->WriteData(0, 64, &transform);

    _lights_buffer = new LightsBuffer;
    _lights_buffer->SetMaxLightsCount(1);
    _lights_buffer->Create();
    _lights_buffer->SetLight(0, 0, 10, 0, 0, Vec3(0, 0, 0), Vec3(0.5, 0.5, 0.5), Color());
    _lights_buffer->UpdateGpuBuffer();

    

    _gbuffer = new VulkanGBufferRenderer;
    _gbuffer->CreateFramebuffer();
    _gbuffer->CreateDescriptorSets();
    _gbuffer->SetEntitiesToRender(_entities_to_render, _particles_to_render);
    _gbuffer->SetCameraIndex(99);
    _gbuffer->Resize(THUMBNAIL_TEXTURE_SIZE, THUMBNAIL_TEXTURE_SIZE);
    _gbuffer->SetBuffers(_transform_buffer, nullptr, nullptr);

    _light = new VulkanDeferredLight;
    _light->CreateFramebuffer();
    _light->CreateDescriptorSet();
    _light->CreatePipeline();
    _light->SetLightsBuffer((VulkanBuffer*)_lights_buffer->GetLightsGpuBuffer());
    _light->SetGBuffer(_gbuffer);
    _light->Resize(THUMBNAIL_TEXTURE_SIZE, THUMBNAIL_TEXTURE_SIZE);
    _light->SetCameraIndex(99);

    _cmdpool = new VulkanCommandPool;
    _cmdpool->Create(device->GetGraphicsQueueFamilyIndex());

    _cmdbuf = new VulkanCommandBuffer;
    _cmdbuf->Create(_cmdpool);

    _begin_semaphore = new VulkanSemaphore();
    _begin_semaphore->Create();
}

void VkMaterialsThumbnails::Destroy() {
    SAFE_RELEASE(_light)
    SAFE_RELEASE(_gbuffer)
}

void VkMaterialsThumbnails::RecreateAll() {

}
void VkMaterialsThumbnails::CreateThumbnail(const std::string& material_name) {
    _queued.push_back(material_name);
}
ImTextureID VkMaterialsThumbnails::GetMaterialThumbnail(const std::string& material_name) {
    for (auto& thumb : _thumbnails) {
        if (thumb.material_name == material_name)
            return thumb.imtexture;
    }
    return nullptr;
}

void VkMaterialsThumbnails::RecordCmdBuffer() {

}
void VkMaterialsThumbnails::CmdExecute(VulkanSemaphore* end) {

}
VulkanSemaphore* VkMaterialsThumbnails::GetBeginSemaphore() {
    return _begin_semaphore;
}