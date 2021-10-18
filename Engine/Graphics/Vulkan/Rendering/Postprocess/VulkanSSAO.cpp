#include "VulkanSSAO.hpp"
#include <Math/Vec3.hpp>
#include <random>
#include <Math/MathBase.hpp>

using namespace VSGE;

VulkanSSAO::VulkanSSAO() {

}
VulkanSSAO::~VulkanSSAO() {
    Destroy();
}

void VulkanSSAO::CreatePrecomputedSSAO() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    std::vector<Vec3> ssaoKernel;
    for (uint32 i = 0; i < 64; ++i)
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
        ssaoKernel.push_back(sample);
    }

    std::vector<Vec3> ssaoNoise;
    for (uint32 i = 0; i < 16; i++)
    {
        Vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    _ssao_noise = new VulkanTexture;
    _ssao_noise->Create(4, 4, FORMAT_RGBA16F, 1, 1);
    _ssao_noise->AddMipLevel((byte*)ssaoNoise.data(), 16 * sizeof(Vec3), 4, 4, 0, 0);
    _ssao_noise->SetReadyToUseInShaders();
}
void VulkanSSAO::Create() {
    CreatePrecomputedSSAO();
}
void VulkanSSAO::Destroy() {

}