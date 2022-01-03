#include "VulkanSSAO.hpp"
#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>
#include <random>
#include <Math/MathBase.hpp>
#include "../VulkanRenderer.hpp"

using namespace VSGE;

#define KERNEL_COUNT 64

VulkanSSAO::VulkanSSAO() {
    _output_size = Vec2i(1280, 720);

    _ssao_noise = nullptr;
    _ssao_kernel = nullptr;

    _ssao_shader = nullptr;
    _ssao_blur_shader = nullptr;
}
VulkanSSAO::~VulkanSSAO() {
    Destroy();
}

void VulkanSSAO::CreatePrecomputedSSAO() {
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
void VulkanSSAO::Create() {
    CreatePrecomputedSSAO();

    _ssao_shader = new VulkanShader;
    _ssao_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
    _ssao_shader->AddShaderFromFile("ssao.frag", SHADER_STAGE_FRAGMENT);

    _ssao_blur_shader = new VulkanShader;
    _ssao_blur_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
    _ssao_blur_shader->AddShaderFromFile("ssao_blur.frag", SHADER_STAGE_FRAGMENT);

}

void VulkanSSAO::Destroy() {

}

void VulkanSSAO::SetInputTexture(Texture* input) {
    PostprocessEffect::SetInputTexture(input);

}

void VulkanSSAO::SetInputTextures(
    Texture* input_positions,
    Texture* input_normals) {

}

VulkanTexture* VulkanSSAO::GetBlurredSSAO() {
    return nullptr;
}

void VulkanSSAO::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {

}

void VulkanSSAO::ResizeOutput(const Vec2i& new_size) {
    if (_output_size == new_size)
        return;

    _output_size = new_size;
}