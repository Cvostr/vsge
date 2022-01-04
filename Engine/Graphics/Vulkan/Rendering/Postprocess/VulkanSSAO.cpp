#include "VulkanSSAO.hpp"
#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>
#include <random>
#include <Math/MathBase.hpp>
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanSSAOBase* ssao_base = nullptr;

#define KERNEL_COUNT 64

VulkanSSAOBase::VulkanSSAOBase() {
    Create();
}
VulkanSSAOBase::~VulkanSSAOBase() {
    Destroy();
}

void VulkanSSAOBase::CreatePrecomputedSSAO() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    //generate SSAO kernel
    std::vector<Vec4> ssaoKernel;
    for (uint32 i = 0; i < KERNEL_COUNT; ++i)
    {
        Vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample.Normalize();
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(Vec4(sample, 0));
    }
    //write kernel vectors to uniform buffer
    _ssao_kernel = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
    _ssao_kernel->Create(sizeof(Vec4) * ssaoKernel.size() + sizeof(Mat4) * 2); //each array member is 16 bytes
    _ssao_kernel->WriteData(0, sizeof(Vec4) * ssaoKernel.size(), ssaoKernel.data());
    //Calculate SSAO noise vectors
    std::vector<Vec4> ssaoNoise;
    for (uint32 i = 0; i < 16; i++)
    {
        Vec4 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f, 0.0f);
        ssaoNoise.push_back(noise);
    }
    //write SSAO noise vectors to texture
    _ssao_noise = new VulkanTexture;
    _ssao_noise->Create(4, 4, FORMAT_RGBA32F, 1, 1);
    _ssao_noise->AddMipLevel((byte*)ssaoNoise.data(),
        ssaoNoise.size() * sizeof(Vec4),
        4, 4, 0, 0);
    _ssao_noise->SetReadyToUseInShaders();
}
void VulkanSSAOBase::Create() {
    CreatePrecomputedSSAO();

    _ssao_shader = new VulkanShader;
    _ssao_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
    _ssao_shader->AddShaderFromFile("ssao.frag", SHADER_STAGE_FRAGMENT);

    _ssao_blur_shader = new VulkanShader;
    _ssao_blur_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
    _ssao_blur_shader->AddShaderFromFile("ssao_blur.frag", SHADER_STAGE_FRAGMENT);

    VulkanDescriptorSet* ssao_base_set = new VulkanDescriptorSet;
    ssao_base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    ssao_base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    ssao_base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
    ssao_base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
    ssao_base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
    ssao_base_set->CreateLayout();
    
    VulkanDescriptorSet* ssao_blur_base_set = new VulkanDescriptorSet;
    ssao_blur_base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    ssao_blur_base_set->CreateLayout();

    _ssao_playout = new VulkanPipelineLayout;
    _ssao_playout->PushDescriptorSet(ssao_base_set);
    _ssao_playout->Create();

    _ssao_blur_playout = new VulkanPipelineLayout;
    _ssao_blur_playout->PushDescriptorSet(ssao_blur_base_set);
    _ssao_blur_playout->Create();

    VertexLayout _vertexLayout;
    _vertexLayout.AddBinding(sizeof(Vertex));
    _vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
    _vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

    VulkanRenderPass* rp = CreateSSAORenderPass();

    _ssao_pipeline = new VulkanPipeline;
    _ssao_pipeline->SetDynamicCullMode(false);
    _ssao_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
    _ssao_pipeline->Create(_ssao_shader, rp, _vertexLayout, _ssao_playout);

    _ssao_blur_pipeline = new VulkanPipeline;
    _ssao_blur_pipeline->SetDynamicCullMode(false);
    _ssao_blur_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
    _ssao_blur_pipeline->Create(_ssao_blur_shader, rp, _vertexLayout, _ssao_blur_playout);

    delete ssao_base_set;
    delete ssao_blur_base_set;
    delete rp;
}

VulkanBuffer* VulkanSSAOBase::GetKernelBuffer() {
    return _ssao_kernel;
}

VulkanTexture* VulkanSSAOBase::GetNoiseTexture() {
    return _ssao_noise;
}

VulkanPipeline* VulkanSSAOBase::GetSSAOPipeline() {
    return _ssao_pipeline;
}
VulkanPipeline* VulkanSSAOBase::GetSSAOBlurPipeline() {
    return _ssao_blur_pipeline;
}

VulkanRenderPass* VulkanSSAOBase::CreateSSAORenderPass() {
    VulkanRenderPass* rp = new VulkanRenderPass;
    rp->SetClearSize(1280, 720);
    rp->PushColorAttachment(FORMAT_RGBA);
    rp->Create();
    return rp;
}

void VulkanSSAOBase::Destroy() {
    SAFE_RELEASE(_ssao_blur_pipeline);
    SAFE_RELEASE(_ssao_pipeline);

    SAFE_RELEASE(_ssao_blur_playout);
    SAFE_RELEASE(_ssao_playout);

    SAFE_RELEASE(_ssao_blur_shader);
    SAFE_RELEASE(_ssao_shader);
}


VulkanSSAO::VulkanSSAO():
    _camera_index(0),
    _descr_pool(nullptr),
    _ssao_descr_set(nullptr),
    _ssao_blur_descr_set(nullptr),
    _rpass(nullptr),
    _fb_ssao(nullptr),
    _fb_ssao_blur(nullptr)
{

}
VulkanSSAO::~VulkanSSAO() {
    Destroy();
}

void VulkanSSAO::Create() {
    if (!ssao_base)
        ssao_base = new VulkanSSAOBase; 
    
    _descr_pool = new VulkanDescriptorPool;

    _ssao_descr_set = new VulkanDescriptorSet(_descr_pool);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
    _ssao_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 4, VK_SHADER_STAGE_FRAGMENT_BIT);

    _ssao_blur_descr_set = new VulkanDescriptorSet(_descr_pool);
    _ssao_blur_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);

    _descr_pool->Create();

    _ssao_descr_set->Create();
    _ssao_blur_descr_set->Create();

    //write images and buffers to ssao descriptor set
    {
        _ssao_descr_set->WriteDescriptorImage(2, ssao_base->GetNoiseTexture(),
            VulkanRenderer::Get()->GetAttachmentSampler());

        _ssao_descr_set->WriteDescriptorBuffer(3, ssao_base->GetKernelBuffer());

        _ssao_descr_set->WriteDescriptorBuffer(4,
            VulkanRenderer::Get()->GetCamerasBuffer()->GetCamerasBuffer());
    }

    _rpass = ssao_base->CreateSSAORenderPass();

    _fb_ssao = new VulkanFramebuffer;
    _fb_ssao->SetSize(1280, 720);
    _fb_ssao->AddAttachment(FORMAT_RGBA);
    _fb_ssao->Create(_rpass);

    _fb_ssao_blur = new VulkanFramebuffer;
    _fb_ssao_blur->SetSize(1280, 720);
    _fb_ssao_blur->AddAttachment(FORMAT_RGBA);
    _fb_ssao_blur->Create(_rpass);
}
void VulkanSSAO::Destroy() {
    SAFE_RELEASE(_fb_ssao_blur);
    SAFE_RELEASE(_fb_ssao);
    SAFE_RELEASE(_rpass);
}

void VulkanSSAO::SetInputTexture(Texture* input) {
    PostprocessEffect::SetInputTexture(input);

}

void VulkanSSAO::SetInputTextures(
    Texture* input_positions,
    Texture* input_normals)
{
    _ssao_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input_positions,
        VulkanRenderer::Get()->GetAttachmentSampler());

    _ssao_descr_set->WriteDescriptorImage(1, (VulkanTexture*)input_normals,
        VulkanRenderer::Get()->GetAttachmentSampler());
}

VulkanTexture* VulkanSSAO::GetBlurredSSAO() {
    return (VulkanTexture*)_fb_ssao_blur->GetColorAttachments()[0];
}

void VulkanSSAO::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
    _rpass->CmdBegin(*cmdbuf, *_fb_ssao);
    cmdbuf->BindPipeline(*ssao_base->GetSSAOPipeline());
    cmdbuf->SetViewport(0, 0, (float)_output_size.x, (float)_output_size.y);
    uint32 cam_offset = _camera_index * CAMERA_ELEM_SIZE;
    cmdbuf->BindDescriptorSets(*ssao_base->GetSSAOPipeline()->GetPipelineLayout(), 0, 1,
        _ssao_descr_set, 1, &cam_offset);
    cmdbuf->BindMesh(*VulkanRenderer::Get()->GetScreenMesh(), 0);
    cmdbuf->DrawIndexed(6);
    cmdbuf->EndRenderPass();

    _rpass->CmdBegin(*cmdbuf, *_fb_ssao_blur);
    cmdbuf->BindPipeline(*ssao_base->GetSSAOBlurPipeline());
    cmdbuf->BindDescriptorSets(*ssao_base->GetSSAOBlurPipeline()->GetPipelineLayout(), 0, 1,
        _ssao_blur_descr_set);
    cmdbuf->DrawIndexed(6);
    cmdbuf->EndRenderPass();
}

void VulkanSSAO::SetCameraIndex(uint32 camera_index) {
    _camera_index = camera_index;
}

void VulkanSSAO::ResizeOutput(const Vec2i& new_size) {
    if (_output_size == new_size)
       return;

    _output_size = new_size;
    _rpass->SetClearSize(new_size.x, new_size.y);
    _fb_ssao->Resize(new_size.x, new_size.y);
    _fb_ssao_blur->Resize(new_size.x, new_size.y);

    _ssao_blur_descr_set->WriteDescriptorImage(0, (VulkanTexture*)_fb_ssao->GetColorAttachments()[0],
        VulkanRenderer::Get()->GetAttachmentSampler());

}