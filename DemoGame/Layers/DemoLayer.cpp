#include "DemoLayer.hpp"
#include <Engine/Application.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/FontManager.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/ColliderComponent.hpp>
#include <Scene/EntityComponents/RigidBodyComponent.hpp>
#include <Scene/EntityComponents/CharacterControllerComponent.hpp>
#include <Scene/EntityComponents/AudioListenerComponent.hpp>
#include <Scene/SceneEnvironment.hpp>
#include <Audio/AudioLayer.hpp>
#include <Core/Logger.hpp>
#include <Math/MathBase.hpp>
#include <UI/Widgets/ImageView.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <utility>
#include <SDL2/SDL.h>

using namespace VSGE;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kDegToRad = kPi / 180.f;
    constexpr float kRadToDeg = 180.f / kPi;
    constexpr float kMaxCameraPitch = 89.f * kDegToRad;
    constexpr float kPlayerGroundHeight = 0.645f;
    constexpr float kPlayerEyeHeight = 0.975f;
    constexpr uint32 kScreenWidth = 1600;
    constexpr uint32 kScreenHeight = 900;

    float AngleDeltaDegrees(float target, float current) {
        float delta = fmodf(target - current + 540.f, 360.f) - 180.f;
        return delta;
    }

    Quat RotationFacing(const Vec3& direction) {
        Vec3 forward = direction.GetNormalized();
        if (forward.z < -0.9999f)
            return Quat(0.f, 1.f, 0.f, 0.f);
        return Quat(-forward.y, forward.x, 0.f, 1.f + forward.z).GetNormalized();
    }

    float Hash2D(uint32 x, uint32 y, uint32 seed) {
        uint32 h = seed ^ (x * 1664525u) ^ (y * 22695477u);
        h ^= h >> 16;
        h *= 2246822519u;
        h ^= h >> 13;
        h *= 3266489917u;
        h ^= h >> 16;
        return (float)(h & 0x00FFFFFFu) / 16777215.f;
    }

    float EndingMusicSample(float t, float phase) {
        const float barTime = fmodf(t, 8.f);
        const int chord = (int)(barTime / 2.f);
        const float chordRoots[] = { 55.f, 65.406f, 73.416f, 49.f };
        const float root = chordRoots[chord];
        const float beatTime = fmodf(barTime, 0.5f);
        const float melodyNote = root * ((int)(barTime * 2.f) % 4 == 0 ? 2.f
            : ((int)(barTime * 2.f) % 4 == 1 ? 2.3784f
            : ((int)(barTime * 2.f) % 4 == 2 ? 3.f : 2.8284f)));
        const float bass = sinf(2.f * kPi * root * t + phase) * 0.22f
            + sinf(2.f * kPi * root * 1.4983f * t + phase * 0.4f) * 0.08f;
        const float pulse = expf(-3.5f * beatTime);
        const float melody = sinf(2.f * kPi * melodyNote * t + phase * 0.2f) * pulse * 0.24f;
        const float throb = sinf(2.f * kPi * 36.f * t) * (0.12f + 0.06f * sinf(2.f * kPi * 0.5f * t));
        return bass + melody + throb;
    }
}

void DemoLayer::OnAttach() {
    _scene = SceneLayer::Get()->GetMainScene();
    if (!_scene) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: no scene available\n";
        return;
    }

    auto& env = _scene->GetEnvironmentSettings();
    env.SetAmbientColor(Color(0.025f, 0.03f, 0.045f, 1.f));
    env.SetMaxShadowDistance(90.f);
    env.SetShadowCascadesCount(4);
    env.UpdateShadows();

    CreateResources();
    BuildManor();
    CreatePlayer();
    CreateArtifacts();
    CreatePickups();
    CreateMonsters();
    CreateAudio();
    CreateHUD();
    SetSkybox();

    _scene->Run();
    IRenderer* renderer = Application::Get()->GetRenderer();
    if (renderer) {
        renderer->SetScene(_scene);
        renderer->QueueSkyProbeUpdate();
    }
    _mouseCaptured = false;
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);

    _state = GameState::Menu;
    _yawDegrees = 180.f;
    _pitch = 0.f;
    UpdateHUD();
}

void DemoLayer::OnDetach() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);
}

float DemoLayer::ThumpSample(float t, float phase) {
    float noise = sinf((t * 971.3f + phase) * 12.9898f) * 43758.5453f;
    noise -= floorf(noise);
    return expf(-18.f * t) * 0.85f * sinf(2.f * kPi * 68.f * t)
        + expf(-28.f * t) * 0.45f * (noise - 0.5f);
}

float DemoLayer::FootstepSample(float t, float phase) {
    const float attack = 1.f - expf(-115.f * t);
    const float body = 0.62f * sinf(2.f * kPi * 68.f * t + phase) * expf(-15.f * t)
        + 0.32f * sinf(2.f * kPi * 138.f * t + phase * 0.6f) * expf(-23.f * t);
    float noise = sinf((t * 821.7f + phase) * 12.9898f) * 43758.5453f;
    noise -= floorf(noise);
    const float heel = 0.28f * sinf(2.f * kPi * 44.f * t) * expf(-11.f * t);
    const float shoeTap = 0.13f * sinf(2.f * kPi * 680.f * t) * expf(-44.f * t);
    return attack * (body + heel + shoeTap + 0.12f * (noise - 0.5f) * expf(-17.f * t));
}

float DemoLayer::FlashlightSwitchSample(float t, float phase) {
    const float click = sinf(2.f * kPi * 1150.f * t + phase) * expf(-48.f * t);
    const float tail = sinf(2.f * kPi * 420.f * t + phase * 0.3f) * expf(-11.f * t);
    return 0.7f * click + 0.3f * tail;
}

float DemoLayer::SwingSample(float t, float phase) {
    float noise = sinf((t * 591.7f + phase) * 78.233f) * 12543.123f;
    noise -= floorf(noise);
    const float sweep = sinf(2.f * kPi * (260.f - 150.f * t) * t + phase) * 0.35f;
    const float envelope = sinf(kPi * std::min(1.f, t / 0.28f)) * expf(-2.2f * t);
    return (sweep + (noise - 0.5f) * 0.65f) * envelope;
}

float DemoLayer::ImpactSample(float t, float phase) {
    const float thud = sinf(2.f * kPi * (92.f - 32.f * t) * t + phase) * expf(-15.f * t);
    const float crack = sinf(2.f * kPi * 710.f * t + phase * 0.4f) * expf(-34.f * t);
    return 0.78f * thud + 0.22f * crack;
}

float DemoLayer::ScareSample(float t, float phase) {
    const float impact = sinf(2.f * kPi * (74.f - 18.f * t) * t + phase) * expf(-5.5f * t);
    const float dissonance = sinf(2.f * kPi * 311.f * t + phase * 0.3f)
        + 0.55f * sinf(2.f * kPi * 419.f * t + phase);
    const float swell = fminf(1.f, t / 0.025f) * expf(-3.2f * t);
    return 0.48f * impact + 0.12f * dissonance * swell;
}

float DemoLayer::ExitScareSample(float t, float phase) {
    const float attack = fminf(1.f, t / 0.012f);
    const float decay = expf(-1.35f * t);
    const float roar = tanhf(2.4f * sinf(2.f * kPi * (68.f * t + 18.f * t * t) + phase)
        + 1.1f * sinf(2.f * kPi * (137.f * t + 31.f * t * t) + phase * 0.4f));
    const float shriek = sinf(2.f * kPi * (310.f * t + 210.f * t * t) + phase * 0.7f)
        + 0.45f * sinf(2.f * kPi * (625.f * t + 330.f * t * t) + phase);
    float noise = sinf((t * 1931.f + phase) * 78.233f) * 12543.123f;
    noise -= floorf(noise);
    const float slam = sinf(2.f * kPi * (48.f - 24.f * t) * t) * expf(-13.f * t);
    return attack * (decay * (0.30f * roar + 0.17f * shriek + 0.12f * (noise - 0.5f)) + 0.42f * slam);
}

float DemoLayer::PickupSample(float t, float phase) {
    float tone = sinf(2.f * kPi * 740.f * t + phase) * expf(-3.2f * t);
    float harmonic = 0.45f * sinf(2.f * kPi * 1480.f * t) * expf(-7.f * t);
    float swell = 0.35f * sinf(2.f * kPi * 990.f * t) * expf(-12.f * fabsf(t - 0.32f));
    return (tone + harmonic + swell) * 0.75f;
}

float DemoLayer::HurtSample(float t, float phase) {
    float tone = tanhf(3.f * sinf(2.f * kPi * 92.f * t + phase));
    float overtone = 0.45f * tanhf(2.5f * sinf(2.f * kPi * 184.f * t + 0.4f));
    return (tone + overtone) * expf(-4.5f * t) * 0.7f;
}

float DemoLayer::DoorSample(float t, float phase) {
    float noise = sinf((t * 613.f + phase) * 78.233f) * 12543.123f;
    noise -= floorf(noise);
    float low = sinf(2.f * kPi * 43.f * t) * 0.65f + sinf(2.f * kPi * 86.f * t + 0.6f) * 0.35f;
    float rumble = (noise - 0.5f) * 0.8f;
    float envelope = fminf(1.f, t / 0.18f) * fminf(1.f, (1.35f - t) / 0.35f);
    return (low + rumble) * envelope * 0.8f;
}

float DemoLayer::StalkerVocalSample(float t, float phase) {
    const float envelope = (1.f - expf(-45.f * t)) * expf(-2.2f * t);
    const float rise = 230.f + 310.f * t;
    const float shriek = sinf(2.f * kPi * rise * t + phase)
        + 0.42f * sinf(2.f * kPi * (rise * 1.91f) * t + phase * 0.6f);
    const float breath = sinf(2.f * kPi * 67.f * t + phase * 0.2f);
    return envelope * (0.32f * shriek + 0.12f * breath);
}

float DemoLayer::BruteVocalSample(float t, float phase) {
    const float envelope = (1.f - expf(-12.f * t)) * expf(-3.1f * t);
    const float fundamental = tanhf(2.4f * sinf(2.f * kPi * (48.f - 12.f * t) * t + phase));
    const float harmonic = tanhf(1.7f * sinf(2.f * kPi * 97.f * t + phase * 0.35f));
    const float rumble = sinf(2.f * kPi * 29.f * t + phase * 0.15f);
    return envelope * (0.30f * fundamental + 0.14f * harmonic + 0.20f * rumble);
}

float DemoLayer::CrawlerVocalSample(float t, float phase) {
    const float fade = expf(-4.2f * t);
    const float clickA = sinf(2.f * kPi * 980.f * t + phase) * expf(-48.f * fmodf(t, 0.19f));
    const float clickB = sinf(2.f * kPi * 1430.f * t + phase * 0.4f)
        * expf(-52.f * fabsf(fmodf(t + 0.09f, 0.27f)));
    const float hiss = sinf(2.f * kPi * (330.f + 90.f * sinf(13.f * t)) * t + phase);
    return fade * (0.21f * clickA + 0.14f * clickB + 0.14f * hiss);
}

float DemoLayer::HealthPickupSample(float t, float phase) {
    const float envelope = expf(-4.f * t);
    return envelope * (0.24f * sinf(2.f * kPi * 392.f * t + phase)
        + 0.20f * sinf(2.f * kPi * 523.25f * t + phase * 0.5f)
        + 0.12f * sinf(2.f * kPi * 659.25f * t));
}

float DemoLayer::StaminaPickupSample(float t, float phase) {
    const float envelope = expf(-3.6f * t);
    const float sweep = 310.f + 390.f * std::min(1.f, t / 0.55f);
    return envelope * (0.22f * sinf(2.f * kPi * sweep * t + phase)
        + 0.14f * sinf(2.f * kPi * (sweep * 1.5f) * t));
}

std::vector<byte> DemoLayer::MakeWavBytes(const std::string& clipName, float seconds, float (*sampleFn)(float, float)) {
    const uint32 sampleRate = 22050;
    const uint16 channels = 1;
    const uint16 bitsPerSample = 16;
    const uint32 sampleCount = (uint32)(seconds * sampleRate);
    const uint32 dataSize = sampleCount * sizeof(int16);
    const uint32 byteRate = sampleRate * channels * bitsPerSample / 8;
    const uint16 blockAlign = channels * bitsPerSample / 8;
    const uint32 riffSize = 36 + dataSize;

    std::vector<byte> bytes;
    bytes.reserve(44 + dataSize);
    auto writeBytes = [&bytes](const char* text, size_t count) {
        bytes.insert(bytes.end(), text, text + count);
    };
    auto write16 = [&bytes](uint16 value) {
        bytes.push_back((byte)(value & 0xff));
        bytes.push_back((byte)((value >> 8) & 0xff));
    };
    auto write32 = [&bytes](uint32 value) {
        bytes.push_back((byte)(value & 0xff));
        bytes.push_back((byte)((value >> 8) & 0xff));
        bytes.push_back((byte)((value >> 16) & 0xff));
        bytes.push_back((byte)((value >> 24) & 0xff));
    };

    writeBytes("RIFF", 4);
    write32(riffSize);
    writeBytes("WAVE", 4);
    writeBytes("fmt ", 4);
    write32(16);
    write16(1);
    write16(channels);
    write32(sampleRate);
    write32(byteRate);
    write16(blockAlign);
    write16(bitsPerSample);
    writeBytes("data", 4);
    write32(dataSize);

    float phase = Hash2D((uint32)clipName.size(), (uint32)clipName[0], 1928371u) * 10.f;
    for (uint32 i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float sample = sampleFn(t, phase);
        sample = std::max(-1.f, std::min(1.f, sample));
        int16 value = (int16)(sample * 32767.f);
        bytes.push_back((byte)(value & 0xff));
        bytes.push_back((byte)((value >> 8) & 0xff));
    }

    return bytes;
}

TextureResource* DemoLayer::MakeTexture(uint32 id, const std::string& name, uint32 width, uint32 height, const std::string& style) {
    std::vector<byte> pixels(width * height * 4);
    auto setPixel = [&](uint32 x, uint32 y, uint8 r, uint8 g, uint8 b, uint8 a = 255) {
        if (x >= width || y >= height)
            return;
        size_t index = ((size_t)y * width + x) * 4;
        pixels[index + 0] = r;
        pixels[index + 1] = g;
        pixels[index + 2] = b;
        pixels[index + 3] = a;
    };

    for (uint32 y = 0; y < height; y++) {
        for (uint32 x = 0; x < width; x++) {
            float n = Hash2D(x, y, 99173u);
            uint8 r = 64, g = 64, b = 64;
            uint8 alpha = 255;

            if (style == "brick") {
                uint32 row = y / 34;
                uint32 offset = (row % 2) * 48u;
                uint32 lx = (x + offset) % 96u;
                uint32 ly = y % 34u;
                bool mortar = ly < 5u || lx < 5u;
                if (mortar) {
                    uint8 v = (uint8)(43 + n * 18.f);
                    r = v; g = (uint8)(v + 3); b = (uint8)(v + 5);
                } else {
                    float brick = Hash2D(x / 96u + row * 17u, row, 5511u);
                    r = (uint8)(76 + brick * 48.f + n * 12.f);
                    g = (uint8)(43 + brick * 22.f + n * 8.f);
                    b = (uint8)(35 + brick * 14.f + n * 6.f);
                }
            } else if (style == "planks") {
                uint32 row = y / 38;
                uint32 offset = (row % 2) * 137u;
                uint32 lx = (x + offset) % 274u;
                uint32 ly = y % 38;
                bool seam = ly < 3u || lx < 4u;
                float grain = sinf((float)x * 0.055f + (float)y * 0.013f) * 0.5f + 0.5f;
                float plank = Hash2D(row, x / 274u, 7311u);
                if (seam) {
                    r = 38; g = 24; b = 14;
                } else {
                    r = (uint8)(82 + plank * 44.f + grain * 13.f + n * 8.f);
                    g = (uint8)(49 + plank * 26.f + grain * 8.f + n * 5.f);
                    b = (uint8)(27 + plank * 14.f + grain * 5.f + n * 3.f);
                }
            } else if (style == "plaster") {
                float blotch = 0.5f + 0.5f * sinf((float)x * 0.035f + Hash2D(y / 24u, 0, 991u) * 4.f) * cosf((float)y * 0.027f);
                uint8 v = (uint8)(48 + n * 22.f + blotch * 18.f);
                r = v; g = (uint8)(v + 2); b = (uint8)(v + 7);
            } else if (style == "wood") {
                uint32 plank = x / 48u;
                uint32 lx = x % 48u;
                float grain = 0.5f + 0.5f * sinf((float)y * 0.12f + (float)plank * 2.1f);
                float tone = Hash2D(plank, 0, 3181u);
                if (lx < 3u) {
                    r = 35; g = 22; b = 12;
                } else {
                    r = (uint8)(65 + tone * 43.f + grain * 12.f + n * 8.f);
                    g = (uint8)(39 + tone * 25.f + grain * 7.f + n * 5.f);
                    b = (uint8)(20 + tone * 13.f + grain * 4.f + n * 3.f);
                }
            } else if (style == "metal") {
                float brush = 0.5f + 0.5f * sinf((float)y * 0.21f + Hash2D(x / 35u, 0, 8123u) * 2.f);
                uint8 v = (uint8)(39 + brush * 26.f + n * 13.f);
                r = (uint8)(v - 3); g = (uint8)(v + 2); b = (uint8)(v + 8);
            } else if (style == "cloth") {
                float weave = ((x % 6u) < 2u) ? 0.68f : 1.f;
                weave *= ((y % 6u) < 2u) ? 0.72f : 1.f;
                uint8 v = (uint8)((118 + n * 48.f) * weave);
                r = v; g = (uint8)(v - 7); b = (uint8)(v - 17);
            } else if (style == "carpet") {
                float diag = fabsf((float)((x + y) % 96u) - 48.f);
                float cross = fabsf((float)((x + 192u - y) % 96u) - 48.f);
                bool pattern = diag < 7.f || cross < 7.f;
                bool border = x < 8u || y < 8u || x >= width - 8u || y >= height - 8u;
                if (border) {
                    r = 112; g = 78; b = 18;
                } else if (pattern) {
                    r = 118; g = 27; b = 18;
                } else {
                    r = (uint8)(57 + n * 14.f); g = (uint8)(15 + n * 7.f); b = (uint8)(18 + n * 8.f);
                }
            } else if (style == "painting") {
                float vignette = 1.f - 0.72f * fminf(1.f, (float)std::abs((int)x - (int)width / 2) / (float)(width / 2));
                float slash = fabsf((float)y - (float)x * 0.42f - 18.f);
                bool bloodStroke = slash < 22.f && x > width / 4 && x < width * 3 / 4;
                bool goldStroke = fabsf((float)y - (float)x * 0.18f - 92.f) < 5.f;
                if (bloodStroke) {
                    r = (uint8)(132 * vignette); g = 10; b = 12;
                } else if (goldStroke) {
                    r = (uint8)(165 * vignette); g = (uint8)(117 * vignette); b = 25;
                } else {
                    r = (uint8)((17 + n * 29.f) * vignette);
                    g = (uint8)((10 + n * 18.f) * vignette);
                    b = (uint8)((13 + n * 22.f) * vignette);
                }
            } else if (style == "blood") {
                const float dripSeed = Hash2D(x / 11u, 3u, 6341u);
                const float dripWidth = 1.5f + dripSeed * 4.f;
                const float dripCenter = (float)(x / 11u) * 11.f + 5.5f;
                const float dripLength = 28.f + dripSeed * 175.f;
                const bool drip = fabsf((float)x - dripCenter) < dripWidth
                    && (float)(y % height) < dripLength;
                const bool smear = fabsf(sinf((float)x * 0.035f + sinf((float)y * 0.018f) * 1.4f)) > 0.91f;
                if (drip || smear) {
                    r = (uint8)(82 + n * 42.f);
                    g = (uint8)(5 + n * 10.f);
                    b = (uint8)(9 + n * 12.f);
                } else {
                    r = (uint8)(20 + n * 18.f);
                    g = (uint8)(9 + n * 7.f);
                    b = (uint8)(12 + n * 9.f);
                }
            } else if (style == "sallow") {
                const float mottling = Hash2D(x / 8u, y / 11u, 8417u);
                const float vein = fabsf(sinf((float)x * 0.045f + sinf((float)y * 0.023f) * 2.2f));
                r = (uint8)(70.f + mottling * 42.f + vein * 15.f);
                g = (uint8)(63.f + mottling * 39.f + vein * 11.f);
                b = (uint8)(62.f + mottling * 43.f + vein * 17.f);
            } else if (style == "bruise") {
                const float mottling = Hash2D(x / 9u, y / 9u, 8441u);
                const float bruise = 0.5f + 0.5f * sinf((float)x * 0.038f + sinf((float)y * 0.031f) * 2.6f);
                r = (uint8)(42.f + mottling * 35.f + bruise * 21.f);
                g = (uint8)(27.f + mottling * 29.f + bruise * 12.f);
                b = (uint8)(52.f + mottling * 48.f + bruise * 34.f);
            } else if (style == "mutant_skin") {
                const float mottling = Hash2D(x / 11u, y / 7u, 9101u);
                const float scar = fabsf(sinf((float)x * 0.043f + sinf((float)y * 0.021f) * 2.8f));
                const float vein = 0.5f + 0.5f * sinf((float)y * 0.12f + sinf((float)x * 0.018f) * 2.4f);
                r = (uint8)(28.f + mottling * 35.f + scar * 16.f + n * 10.f);
                g = (uint8)(31.f + mottling * 38.f + vein * 12.f + n * 9.f);
                b = (uint8)(33.f + mottling * 42.f + scar * 17.f + n * 12.f);
            } else if (style == "brain") {
                float fold = 0.5f + 0.5f * sinf((float)x * 0.12f + sinf((float)y * 0.08f) * 2.4f);
                float speckle = Hash2D(x / 9u, y / 9u, 9147u);
                r = (uint8)(112.f + fold * 58.f + speckle * 24.f);
                g = (uint8)(38.f + fold * 35.f + speckle * 16.f);
                b = (uint8)(46.f + fold * 38.f + speckle * 17.f);
            } else if (style == "eyes" || style == "eyes_emission") {
                float dx = (float)x - (float)width * 0.5f;
                float dy = (float)y - (float)height * 0.5f;
                float iris = dx * dx + dy * dy;
                bool glow = iris < (float)(width * width) * 0.045f;
                if (style == "eyes_emission") {
                    uint8 value = glow ? 255 : 0;
                    r = value; g = (uint8)(value * 0.08f); b = (uint8)(value * 0.04f);
                } else {
                    r = glow ? 188 : 28;
                    g = glow ? 18 : 10;
                    b = glow ? 12 : 12;
                }
            } else if (style == "artifact" || style == "artifact_emission") {
                float dx = (float)x - (float)width * 0.5f;
                float dy = (float)y - (float)height * 0.5f;
                float diamond = fabsf(dx) + fabsf(dy);
                bool outerRune = diamond > 68.f && diamond < 80.f;
                bool innerRune = fabsf(dx) < 5.f && fabsf(dy) < 55.f;
                bool crossRune = fabsf(dy) < 5.f && fabsf(dx) < 55.f;
                bool cornerRune = (fabsf(fabsf(dx) - 38.f) < 5.f && fabsf(fabsf(dy) - 38.f) < 5.f);
                bool rune = outerRune || innerRune || crossRune || cornerRune;
                if (style == "artifact_emission") {
                    uint8 v = rune ? 255 : 0;
                    r = g = b = v;
                } else {
                    r = rune ? 72 : (uint8)(12 + n * 12.f);
                    g = rune ? 236 : (uint8)(30 + n * 20.f);
                    b = rune ? 218 : (uint8)(39 + n * 24.f);
                }
            } else if (style == "sky") {
                float vertical = (float)y / (float)height;
                float horizon = expf(-powf((vertical - 0.72f) / 0.18f, 2.f));
                float cloud = 0.5f + 0.5f * sinf((float)x * 0.021f + sinf((float)y * 0.05f) * 2.f);
                uint8 baseR = (uint8)(2 + 9 * vertical + horizon * 16.f);
                uint8 baseG = (uint8)(4 + 14 * vertical + horizon * 28.f);
                uint8 baseB = (uint8)(12 + 25 * vertical + horizon * 48.f);
                if (cloud > 0.72f && vertical > 0.38f && vertical < 0.78f) {
                    baseR = (uint8)std::min(70.f, baseR + 11.f);
                    baseG = (uint8)std::min(78.f, baseG + 13.f);
                    baseB = (uint8)std::min(95.f, baseB + 15.f);
                }
                if (Hash2D(x / 2u, y / 2u, 6181u) > 0.992f && vertical < 0.58f) {
                    r = g = b = 220;
                } else {
                    r = baseR; g = baseG; b = baseB;
                }
            } else if (style == "eerie" || style == "eerie_emission") {
                bool frame = x < 5u || y < 5u || x >= width - 5u || y >= height - 5u;
                bool cross = (x >= width / 2 - 2u && x <= width / 2 + 2u) || (y >= height / 2 - 2u && y <= height / 2 + 2u);
                if (style == "eerie_emission") {
                    uint8 v = (frame || cross) ? 210 : 28;
                    r = g = b = v;
                } else {
                    r = frame ? 38 : (uint8)(24 + n * 16.f);
                    g = frame ? 58 : (uint8)(40 + n * 28.f);
                    b = frame ? 72 : (uint8)(73 + n * 60.f);
                }
            } else if (style == "flame" || style == "flame_emission") {
                float dx = (float)x - width * 0.5f;
                float dy = (float)y - height * 0.55f;
                float distance = sqrtf(dx * dx + dy * dy) / (width * 0.48f);
                bool core = distance < 0.38f;
                bool tongue = fabsf(dx) < (height * 0.24f - y * 0.11f) && y < height * 0.72f;
                if (style == "flame_emission") {
                    uint8 v = (core || tongue) ? 255 : 0;
                    r = g = b = v;
                } else {
                    r = core ? 255 : (tongue ? 239 : (uint8)(72 + n * 40.f));
                    g = core ? 192 : (tongue ? 104 : (uint8)(30 + n * 25.f));
                    b = core ? 58 : (tongue ? 24 : (uint8)(12 + n * 12.f));
                }
            } else if (style == "blood_splatter") {
                const float nx = ((float)x / (float)width - 0.52f) * 2.f;
                const float ny = ((float)y / (float)height - 0.47f) * 2.f;
                const float radius = sqrtf(nx * nx + ny * ny);
                const float angle = atan2f(ny, nx);
                const float spokePosition = angle * 20.f / (2.f * kPi);
                const float spokeIndex = floorf(spokePosition + 0.5f);
                const float spokeOffset = fabsf(spokePosition - spokeIndex);
                const float spokeSeed = Hash2D((uint32)(spokeIndex + 40.f), 1u, 4821u);
                const float spokeLength = 0.32f + spokeSeed * 0.72f;
                const float spokeWidth = 0.012f + spokeSeed * 0.018f;
                const float irregularity = sinf(angle * 17.f + n * 5.f) * 0.025f;
                bool blood = radius < 0.27f + irregularity;
                blood = blood || (radius > 0.10f && radius < spokeLength
                    && spokeOffset < spokeWidth / std::max(radius, 0.08f));

                for (uint32 drop = 0; drop < 18 && !blood; drop++) {
                    const float seedX = Hash2D(drop, 3u, 7811u);
                    const float seedY = Hash2D(drop, 7u, 1171u);
                    const float centerX = (seedX - 0.5f) * 1.9f;
                    const float centerY = (seedY - 0.5f) * 1.8f;
                    const float dropRadius = 0.018f + Hash2D(drop, 11u, 6431u) * 0.045f;
                    const float dx = nx - centerX;
                    const float dy = ny - centerY;
                    blood = (dx * dx) / (dropRadius * dropRadius)
                        + (dy * dy) / (dropRadius * dropRadius * 1.8f) < 1.f;
                }

                if (blood) {
                    const float variation = n * 35.f;
                    r = (uint8)(112.f + variation);
                    g = (uint8)(3.f + variation * 0.10f);
                    b = (uint8)(8.f + variation * 0.12f);
                    alpha = radius < 0.25f ? 235 : (uint8)(150.f + n * 90.f);
                } else {
                    alpha = 0;
                }
            }

            setPixel(x, y, r, g, b, alpha);
        }
    }

    auto* texture = new TextureResource();
    texture->SetName(name);
    texture->SetId(Guid(0, 0, 2, id));
    texture->GetTexture()->Create(width, height, FORMAT_RGBA, 1, 1);
    texture->GetTexture()->AddMipLevel(pixels.data(), (uint32)pixels.size(), width, height, 0, 0);
    texture->GetTexture()->SetReadyToUseInShaders();
    texture->SetState(RESOURCE_STATE_READY);
    ResourceCache::Get()->PushResource(texture);
    return texture;
}

MaterialResource* DemoLayer::MakeMaterial(const std::string& name,
                                          uint32 id,
                                          TextureResource* albedo,
                                          const Color& color,
                                          float roughness,
                                          float metallic,
                                          float emission,
                                          TextureResource* emissionMap) {
    auto* material = new MaterialResource();
    material->SetName(name);
    material->SetId(Guid(0, 0, 4, id));
    material->GetMaterial()->SetTemplate("default_pbr");

    if (albedo) {
        ResourceReference albedoRef;
        albedoRef.SetResource(albedo);
        material->GetMaterial()->SetTexture("Albedo", albedoRef);
    }
    if (emissionMap) {
        ResourceReference emissionRef;
        emissionRef.SetResource(emissionMap);
        material->GetMaterial()->SetTexture("Emission", emissionRef);
    }

    material->GetMaterial()->SetParameter("Color", color);
    material->GetMaterial()->SetParameter("Roughness factor", roughness);
    material->GetMaterial()->SetParameter("Metallic factor", metallic);
    material->GetMaterial()->SetParameter("Height factor", 1.f);
    material->GetMaterial()->SetParameter("Emission factor", emission);
    material->SetState(RESOURCE_STATE_READY);
    ResourceCache::Get()->PushResource(material);
    return material;
}

void DemoLayer::CreateResources() {
    auto* wall = MakeTexture(1, "WallBrick", 512, 512, "brick");
    auto* floor = MakeTexture(2, "WoodFloor", 512, 512, "planks");
    auto* ceiling = MakeTexture(3, "CeilingPlaster", 256, 256, "plaster");
    auto* wood = MakeTexture(4, "AgedWood", 256, 256, "wood");
    auto* metal = MakeTexture(5, "DarkIron", 256, 256, "metal");
    auto* cloth = MakeTexture(6, "LinenCloth", 256, 256, "cloth");
    auto* carpet = MakeTexture(7, "RedCarpet", 256, 256, "carpet");
    auto* painting = MakeTexture(8, "OiledPortrait", 512, 256, "painting");
    auto* monster = MakeTexture(9, "ScarredMutantHide", 256, 256, "mutant_skin");
    auto* brain = MakeTexture(17, "ExposedTissue", 256, 256, "brain");
    auto* eyes = MakeTexture(19, "PredatorEyes", 128, 128, "eyes");
    auto* eyesEmission = MakeTexture(20, "PredatorEyesGlow", 128, 128, "eyes_emission");
    auto* artifactAlbedo = MakeTexture(10, "CursedRelic", 256, 256, "artifact");
    auto* artifactEmission = MakeTexture(11, "CursedRelicGlow", 256, 256, "artifact_emission");
    auto* sky = MakeTexture(12, "NightSky", 512, 512, "sky");
    auto* eerie = MakeTexture(13, "MoonWindow", 256, 256, "eerie");
    auto* eerieEmission = MakeTexture(14, "MoonWindowGlow", 256, 256, "eerie_emission");
    auto* flame = MakeTexture(15, "CandleFlame", 128, 128, "flame");
    auto* flameEmission = MakeTexture(16, "CandleFlameGlow", 128, 128, "flame_emission");
    auto* blood = MakeTexture(21, "OldBloodSmears", 256, 256, "blood");
    auto* sallowHide = MakeTexture(22, "SallowMutantSkin", 256, 256, "sallow");
    auto* bruisedHide = MakeTexture(23, "BruisedMutantSkin", 256, 256, "bruise");
    MakeTexture(24, "EndingBloodOverlay", 640, 360, "blood_splatter");

    _assets.wall = MakeMaterial("ManorWall", 1, wall, Color(0.78f, 0.72f, 0.68f, 1.f), 0.92f, 0.0f);
    _assets.floor = MakeMaterial("ManorFloor", 2, floor, Color(0.72f, 0.60f, 0.48f, 1.f), 0.78f, 0.0f);
    _assets.ceiling = MakeMaterial("ManorCeiling", 3, ceiling, Color(0.52f, 0.51f, 0.49f, 1.f), 0.96f, 0.0f);
    _assets.wood = MakeMaterial("AgedWood", 4, wood, Color(0.66f, 0.50f, 0.35f, 1.f), 0.72f, 0.0f);
    _assets.darkMetal = MakeMaterial("DarkIron", 5, metal, Color(0.48f, 0.50f, 0.55f, 1.f), 0.34f, 0.86f);
    _assets.mutantBase = MakeMaterial("ScarredMutantHide", 6, monster, Color(0.30f, 0.34f, 0.31f, 1.f), 0.72f, 0.02f);
    _assets.mutantCarapace = MakeMaterial("MutantCarapace", 15, monster, Color(0.19f, 0.24f, 0.23f, 1.f), 0.52f, 0.08f);
    _assets.brain = MakeMaterial("ExposedTissue", 14, brain, Color(0.62f, 0.20f, 0.24f, 1.f), 0.68f, 0.0f);
    _assets.eyes = MakeMaterial("PredatorEyes", 17, eyes, Color(1.f, 0.22f, 0.18f, 1.f), 0.18f, 0.05f, 3.2f, eyesEmission);
    const Color hideColors[3] = {
        Color(0.25f, 0.32f, 0.28f, 1.f),
        Color(0.39f, 0.34f, 0.29f, 1.f),
        Color(0.13f, 0.27f, 0.30f, 1.f)
    };
    const Color rindColors[3] = {
        Color(0.16f, 0.22f, 0.19f, 1.f),
        Color(0.32f, 0.25f, 0.19f, 1.f),
        Color(0.08f, 0.20f, 0.25f, 1.f)
    };
    const Color brainColors[3] = {
        Color(0.39f, 0.10f, 0.12f, 1.f),
        Color(0.27f, 0.14f, 0.17f, 1.f),
        Color(0.25f, 0.10f, 0.19f, 1.f)
    };
    const Color eyeColors[3] = {
        Color(0.78f, 0.08f, 0.025f, 1.f),
        Color(0.72f, 0.47f, 0.12f, 1.f),
        Color(0.62f, 0.68f, 0.54f, 1.f)
    };
    TextureResource* hideTextures[3] = { monster, sallowHide, bruisedHide };
    for (uint32 i = 0; i < 3; i++) {
        const uint32 id = 21 + i * 5;
        const std::string suffix = std::to_string(i + 1);
        _assets.mutantHide[i] = MakeMaterial("MutantHide" + suffix, id, hideTextures[i], hideColors[i], 0.48f, 0.02f);
        _assets.mutantCarapaceByArchetype[i] = MakeMaterial("MutantCarapace" + suffix, id + 1, hideTextures[i], rindColors[i], 0.42f, 0.f);
        _assets.mutantBrain[i] = MakeMaterial("MutantBrain" + suffix, id + 3, brain, brainColors[i], 0.56f, 0.f);
        _assets.mutantEyes[i] = MakeMaterial("MutantEyes" + suffix, id + 4, eyes, eyeColors[i], 0.22f, 0.02f, 1.1f, eyesEmission);
        _assets.mutantMouth[i] = MakeMaterial("MutantMouth" + suffix, 38 + i, brain, Color(0.12f, 0.018f, 0.025f, 1.f), 0.72f, 0.f);
    }
    _assets.mutantBone = MakeMaterial("MutantOldBone", 37, cloth, Color(0.48f, 0.40f, 0.28f, 1.f), 0.84f, 0.f);
    _assets.healthPickup = MakeMaterial("BloodTonic", 41, blood, Color(0.86f, 0.12f, 0.16f, 1.f), 0.28f, 0.f, 1.1f, artifactEmission);
    _assets.staminaPickup = MakeMaterial("GreenSalts", 42, eerie, Color(0.26f, 0.78f, 0.44f, 1.f), 0.28f, 0.f, 1.2f, eerieEmission);
    _assets.horrorBlood = MakeMaterial("ManorBlood", 36, blood, Color(0.82f, 0.48f, 0.46f, 1.f), 0.68f, 0.f);
    _assets.artifact = MakeMaterial("CursedRelic", 7, artifactAlbedo, Color(0.84f, 0.96f, 0.93f, 1.f), 0.22f, 0.35f, 3.3f, artifactEmission);
    _assets.carpet = MakeMaterial("RedCarpet", 8, carpet, Color(0.72f, 0.48f, 0.42f, 1.f), 0.96f, 0.0f);
    _assets.painting = MakeMaterial("OiledPortrait", 9, painting, Color(0.74f, 0.66f, 0.58f, 1.f), 0.88f, 0.0f);
    _assets.cloth = MakeMaterial("LinenCloth", 10, cloth, Color(0.75f, 0.74f, 0.68f, 1.f), 0.97f, 0.0f);
    _assets.door = MakeMaterial("NorthDoor", 11, wood, Color(0.40f, 0.26f, 0.18f, 1.f), 0.64f, 0.12f);
    _assets.eerie = MakeMaterial("MoonWindow", 12, eerie, Color(0.62f, 0.76f, 0.90f, 1.f), 0.52f, 0.08f, 2.0f, eerieEmission);
    _assets.flame = MakeMaterial("CandleFlame", 13, flame, Color(1.0f, 0.86f, 0.62f, 1.f), 0.30f, 0.0f, 4.0f, flameEmission);
}

void DemoLayer::CreateAudio() {
    struct SoundDef {
        const char* name;
        uint32 id;
        float seconds;
        float (*sample)(float, float);
    };

    const SoundDef sounds[] = {
        { "thump",         2, 0.72f, &ThumpSample },
        { "pickup",        3, 0.95f, &PickupSample },
        { "hurt",          4, 0.78f, &HurtSample },
        { "door_open",     5, 1.35f, &DoorSample },
        { "footstep",      6, 0.20f, &FootstepSample },
        { "scare",         7, 0.90f, &ScareSample },
        { "swing",         8, 0.32f, &SwingSample },
        { "impact",        9, 0.42f, &ImpactSample },
        { "flashlight",   10, 0.24f, &FlashlightSwitchSample },
        { "exit_scare",   11, 2.4f, &ExitScareSample },
        { "stalker_vocal", 12, 1.15f, &StalkerVocalSample },
        { "brute_vocal",   13, 1.30f, &BruteVocalSample },
        { "crawler_vocal", 14, 1.05f, &CrawlerVocalSample },
        { "health_pickup", 15, 0.85f, &HealthPickupSample },
        { "stamina_pickup",16, 0.85f, &StaminaPickupSample },
    };

    for (const auto& sound : sounds) {
        std::vector<byte> wav = MakeWavBytes(sound.name, sound.seconds, sound.sample);
        auto* clip = new AudioClipResource();
        clip->SetName(sound.name);
        clip->SetId(Guid(0, 0, 7, sound.id));
        if (clip->GetAudioBuffer()->loadBuffer(wav.data(), wav.size())) {
            clip->SetDefault();
            clip->SetState(RESOURCE_STATE_READY);
            ResourceCache::Get()->PushResource(clip);
        } else {
            Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: failed creating fallback sound '" << sound.name << "'\n";
            delete clip;
        }
    }

    auto loadOggClip = [](const std::filesystem::path& path, const std::string& name, uint32 id) -> AudioClipResource* {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: could not open OGG sound '" << path.string() << "'\n";
            return nullptr;
        }

        const std::streamsize fileSize = file.tellg();
        if (fileSize <= 0) {
            Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: OGG sound is empty '" << path.string() << "'\n";
            return nullptr;
        }

        std::vector<byte> oggData(static_cast<size_t>(fileSize));
        file.seekg(0, std::ios::beg);
        if (!file.read(reinterpret_cast<char*>(oggData.data()), fileSize)) {
            Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: failed reading OGG sound '" << path.string() << "'\n";
            return nullptr;
        }

        auto* clip = new AudioClipResource();
        clip->SetName(name);
        clip->SetId(Guid(0, 0, 9, id));
        if (!clip->GetAudioBuffer()->loadBuffer(oggData.data(), oggData.size())) {
            Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: failed decoding OGG sound '" << path.string() << "'\n";
            delete clip;
            return nullptr;
        }

        clip->SetDefault();
        clip->SetState(RESOURCE_STATE_READY);
        ResourceCache::Get()->PushResource(clip);
        return clip;
    };

    const char* audioCategories[] = {
        "steps", "attack", "monsters_attack", "monsters", "ambient",
        "pickups", "scare", "doors", "ui", "ending"
    };
    uint32 customClipId = 100;
    for (const char* category : audioCategories) {
        const std::filesystem::path folder = std::filesystem::path("sounds") / category;
        std::error_code error;
        if (!std::filesystem::is_directory(folder, error)) {
            if (error)
                Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: cannot inspect sound folder '" << folder.string() << "'\n";
            continue;
        }

        std::vector<std::filesystem::path> oggFiles;
        for (std::filesystem::directory_iterator it(folder, error), end; !error && it != end; it.increment(error)) {
            if (!it->is_regular_file(error) || error)
                continue;
            std::string extension = it->path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(),
                           [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            if (extension == ".ogg")
                oggFiles.push_back(it->path());
        }
        if (error) {
            Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: failed reading sound folder '" << folder.string() << "'\n";
            continue;
        }

        std::sort(oggFiles.begin(), oggFiles.end());
        for (const auto& path : oggFiles) {
            const std::string clipName = std::string("custom_") + category + "_" + std::to_string(customClipId);
            if (AudioClipResource* clip = loadOggClip(path, clipName, customClipId)) {
                _customAudioClips[category].push_back(clip);
                Logger::Log(LogType::LOG_TYPE_INFO) << "House of Shadows: loaded custom sound '" << path.string() << "'\n";
            }
            customClipId++;
        }
    }

    _ambientMusicClip = GetCustomAudioClip("ambient");
    if (!_ambientMusicClip)
        _ambientMusicClip = loadOggClip("sounds/duhovoy-vopl--latunnaya-pechal.ogg", "ambient_music", 1);
    if (!_ambientMusicClip)
        Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: no ambient music found; continuing without background music\n";

    _ambientEntity = _scene->AddNewEntity("AmbientAudio");
    _ambientSource = _ambientEntity->AddComponent<AudioSourceComponent>();
    _ambientSource->SetLoop(true);
    _ambientSource->SetVolume(0.58f);
    _ambientSource->SetPitch(1.f);
    if (_ambientMusicClip) {
        _ambientSource->SetAudioClip(_ambientMusicClip);
        if (_cameraEntity)
            _cameraEntity->AddChild(_ambientEntity, false);
        _ambientSource->Play();
        if (_ambientSource->IsPlaying()) {
            Logger::Log(LogType::LOG_TYPE_INFO) << "House of Shadows: background music playing, loop enabled\n";
        } else {
            Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: background music source did not start\n";
        }
    }

    _endingMusicClip = GetCustomAudioClip("ending");
    if (!_endingMusicClip) {
        auto* endingClip = new AudioClipResource();
        endingClip->SetName("ending_music");
        endingClip->SetId(Guid(0, 0, 7, 2));
        std::vector<byte> endingWav = MakeWavBytes("ending_music", 8.f, &EndingMusicSample);
        if (endingClip->GetAudioBuffer()->loadBuffer(endingWav.data(), endingWav.size())) {
            endingClip->SetDefault();
            endingClip->SetState(RESOURCE_STATE_READY);
            ResourceCache::Get()->PushResource(endingClip);
            _endingMusicClip = endingClip;
        } else {
            Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: failed to create ending music\n";
            delete endingClip;
        }
    }

    _sfxEntity = _scene->AddNewEntity("SfxAudio");
    _sfxSource = _sfxEntity->AddComponent<AudioSourceComponent>();
    _sfxSource->SetVolume(0.8f);
    if (_cameraEntity)
        _cameraEntity->AddChild(_sfxEntity, false);

    _footstepEntity = _scene->AddNewEntity("FootstepAudio");
    _footstepSource = _footstepEntity->AddComponent<AudioSourceComponent>();
    _footstepSource->SetVolume(0.95f);
    if (_cameraEntity)
        _cameraEntity->AddChild(_footstepEntity, false);
}

Entity* DemoLayer::AddStaticBox(const std::string& name,
                                const Vec3& position,
                                const Vec3& scale,
                                MaterialResource* material,
                                bool castShadows) {
    Entity* entity = _scene->AddNewEntity(name);
    entity->SetPosition(position);
    entity->SetScale(scale);
    entity->AddComponent<MeshComponent>()->SetMeshResource(GetCubeMesh());
    auto* materialComponent = entity->AddComponent<MaterialComponent>();
    materialComponent->SetMaterialResource(material);
    materialComponent->SetCastShadows(castShadows);

    auto* collider = entity->AddComponent<ColliderComponent>();
    collider->SetColliderShape(ColliderShape::COLLIDER_SHAPE_CUBE);
    collider->SetSize(Vec3(1.f, 1.f, 1.f));
    collider->AddToWorld();
    return entity;
}

void DemoLayer::BuildManor() {
    auto AddDecorBox = [this](const std::string& name, const Vec3& position, const Vec3& scale, MaterialResource* material, const Vec3& rotation = Vec3(0.f)) {
        Entity* entity = _scene->AddNewEntity(name);
        entity->SetPosition(position);
        entity->SetScale(scale);
        entity->SetRotationEuler(rotation);
        entity->AddComponent<MeshComponent>()->SetMeshResource(GetCubeMesh());
        auto* materialComponent = entity->AddComponent<MaterialComponent>();
        materialComponent->SetMaterialResource(material);
        materialComponent->SetCastShadows(false);
        return entity;
    };

    // Architecture: one floor, two side wings, a long central hall and a sealed north exit.
    AddStaticBox("Floor", Vec3(0.f, -0.5f, 0.f), Vec3(17.5f, 0.5f, 12.1f), _assets.floor, false);
    AddStaticBox("Ceiling", Vec3(0.f, 4.68f, 0.f), Vec3(17.5f, 0.22f, 12.1f), _assets.ceiling, false);

    AddStaticBox("NorthWallWest", Vec3(-9.2f, 2.25f, -12.0f), Vec3(8.0f, 4.5f, 0.35f), _assets.wall);
    AddStaticBox("NorthWallEast", Vec3(9.2f, 2.25f, -12.0f), Vec3(8.0f, 4.5f, 0.35f), _assets.wall);
    AddStaticBox("SouthWall", Vec3(0.f, 2.25f, 12.0f), Vec3(17.5f, 4.5f, 0.35f), _assets.wall);
    AddStaticBox("WestWall", Vec3(-17.0f, 2.25f, 0.f), Vec3(0.35f, 4.5f, 12.0f), _assets.wall);
    AddStaticBox("EastWall", Vec3(17.0f, 2.25f, 0.f), Vec3(0.35f, 4.5f, 12.0f), _assets.wall);

    AddStaticBox("DoorFrameWest", Vec3(-1.42f, 1.75f, -11.75f), Vec3(0.18f, 1.75f, 0.18f), _assets.wood);
    AddStaticBox("DoorFrameEast", Vec3(1.42f, 1.75f, -11.75f), Vec3(0.18f, 1.75f, 0.18f), _assets.wood);
    AddStaticBox("DoorLintel", Vec3(0.f, 3.52f, -11.75f), Vec3(1.58f, 0.20f, 0.18f), _assets.wood);
    _exitDoor = AddStaticBox("SealedNorthDoor", Vec3(0.f, 1.62f, -11.72f), Vec3(1.23f, 1.65f, 0.12f), _assets.door);

    // Vertical partitions. Door gaps at z=-4 and z=4 connect the hall to the north and south wings.
    const float partitionX[2] = { -5.f, 5.f };
    for (int side = 0; side < 2; side++) {
        float x = partitionX[side];
        std::string prefix = side == 0 ? "WestPartition" : "EastPartition";
        AddStaticBox(prefix + "North", Vec3(x, 2.25f, -8.45f), Vec3(0.18f, 4.5f, 3.55f), _assets.wall);
        AddStaticBox(prefix + "Middle", Vec3(x, 2.25f, 0.f), Vec3(0.18f, 4.5f, 3.10f), _assets.wall);
        AddStaticBox(prefix + "South", Vec3(x, 2.25f, 8.45f), Vec3(0.18f, 4.5f, 3.55f), _assets.wall);
    }

    // Horizontal room dividers with doorways facing the central hall.
    AddStaticBox("WestDividerLeft", Vec3(-12.5f, 2.25f, 0.f), Vec3(4.6f, 4.5f, 0.18f), _assets.wall);
    AddStaticBox("WestDividerRight", Vec3(-5.55f, 2.25f, 0.f), Vec3(0.45f, 4.5f, 0.18f), _assets.wall);
    AddStaticBox("EastDividerLeft", Vec3(5.55f, 2.25f, 0.f), Vec3(0.45f, 4.5f, 0.18f), _assets.wall);
    AddStaticBox("EastDividerRight", Vec3(12.5f, 2.25f, 0.f), Vec3(4.6f, 4.5f, 0.18f), _assets.wall);

    // Central hall: carpet, banquet table, statues and portraits.
    AddDecorBox("CentralCarpet", Vec3(0.f, 0.025f, 0.f), Vec3(2.6f, 0.025f, 5.0f), _assets.carpet);
    AddStaticBox("BanquetTable", Vec3(0.f, 0.48f, -0.2f), Vec3(1.75f, 0.48f, 0.78f), _assets.wood);
    AddStaticBox("BanquetLegWest", Vec3(-1.35f, 0.28f, -0.2f), Vec3(0.16f, 0.28f, 0.52f), _assets.wood);
    AddStaticBox("BanquetLegEast", Vec3(1.35f, 0.28f, -0.2f), Vec3(0.16f, 0.28f, 0.52f), _assets.wood);
    AddStaticBox("NorthStatueWest", Vec3(-3.7f, 1.25f, -5.2f), Vec3(0.48f, 1.25f, 0.48f), _assets.darkMetal);
    AddStaticBox("NorthStatueEast", Vec3(3.7f, 1.25f, -5.2f), Vec3(0.48f, 1.25f, 0.48f), _assets.darkMetal);
    AddStaticBox("SouthStatueWest", Vec3(-3.7f, 1.25f, 6.2f), Vec3(0.48f, 1.25f, 0.48f), _assets.darkMetal);
    AddStaticBox("SouthStatueEast", Vec3(3.7f, 1.25f, 6.2f), Vec3(0.48f, 1.25f, 0.48f), _assets.darkMetal);
    AddDecorBox("PortraitHallWest", Vec3(-4.76f, 2.25f, 2.8f), Vec3(0.05f, 0.78f, 1.15f), _assets.painting);
    AddDecorBox("PortraitHallEast", Vec3(4.76f, 2.25f, 2.8f), Vec3(0.05f, 0.78f, 1.15f), _assets.painting);

    // West bedroom: bed, wardrobe, side table.
    AddStaticBox("BedBase", Vec3(-12.5f, 0.34f, 7.1f), Vec3(2.05f, 0.34f, 2.95f), _assets.wood);
    AddStaticBox("Mattress", Vec3(-12.5f, 0.78f, 7.1f), Vec3(1.90f, 0.18f, 2.78f), _assets.cloth, false);
    AddStaticBox("Pillow", Vec3(-12.5f, 1.02f, 8.85f), Vec3(1.30f, 0.16f, 0.55f), _assets.cloth, false);
    AddStaticBox("Wardrobe", Vec3(-16.0f, 1.45f, 7.3f), Vec3(0.58f, 1.45f, 1.55f), _assets.wood);
    AddStaticBox("BedroomTable", Vec3(-8.0f, 0.42f, 8.6f), Vec3(1.1f, 0.42f, 0.72f), _assets.wood);
    AddStaticBox("BedroomChair", Vec3(-8.0f, 0.42f, 6.6f), Vec3(0.34f, 0.42f, 0.34f), _assets.wood);

    // West study: shelves, desk and chair.
    AddStaticBox("StudyShelfNorth", Vec3(-16.0f, 2.0f, -7.0f), Vec3(0.42f, 2.0f, 2.3f), _assets.wood);
    AddStaticBox("StudyShelfWest", Vec3(-12.0f, 2.0f, -11.45f), Vec3(2.4f, 2.0f, 0.28f), _assets.wood);
    AddStaticBox("StudyDesk", Vec3(-11.0f, 0.52f, -6.7f), Vec3(1.55f, 0.52f, 0.82f), _assets.wood);
    AddStaticBox("StudyChair", Vec3(-11.0f, 0.43f, -4.8f), Vec3(0.35f, 0.43f, 0.35f), _assets.wood);
    AddStaticBox("StudyCabinet", Vec3(-6.2f, 0.78f, -10.7f), Vec3(0.62f, 0.78f, 1.05f), _assets.wood);

    // East library: shelves and a reading table.
    AddStaticBox("LibraryShelfEast", Vec3(16.0f, 2.1f, -7.4f), Vec3(0.45f, 2.1f, 2.8f), _assets.wood);
    AddStaticBox("LibraryShelfNorth", Vec3(10.2f, 2.1f, -11.45f), Vec3(2.8f, 2.1f, 0.28f), _assets.wood);
    AddStaticBox("LibraryTable", Vec3(11.2f, 0.52f, -7.1f), Vec3(1.45f, 0.52f, 0.78f), _assets.wood);
    AddStaticBox("LibraryChair", Vec3(11.2f, 0.43f, -5.2f), Vec3(0.35f, 0.43f, 0.35f), _assets.wood);
    AddStaticBox("GlobeStand", Vec3(7.0f, 0.72f, -9.5f), Vec3(0.28f, 0.72f, 0.28f), _assets.wood);

    // East cellar: stacked industrial crates.
    AddStaticBox("CrateLargeA", Vec3(10.2f, 0.62f, 8.0f), Vec3(1.35f, 0.62f, 1.20f), _assets.wood);
    AddStaticBox("CrateLargeB", Vec3(13.6f, 0.72f, 7.2f), Vec3(1.1f, 0.72f, 1.1f), _assets.wood);
    AddStaticBox("CrateStackTop", Vec3(13.6f, 1.78f, 7.2f), Vec3(0.82f, 0.34f, 0.82f), _assets.wood);
    AddStaticBox("CrateLong", Vec3(15.2f, 0.45f, 9.6f), Vec3(1.45f, 0.45f, 0.55f), _assets.wood);
    AddStaticBox("CellarWorkbench", Vec3(8.0f, 0.52f, 10.5f), Vec3(1.75f, 0.52f, 0.62f), _assets.wood);

    // Broken furniture and clutter, arranged to leave readable escape paths.
    AddStaticBox("FallenShelf", Vec3(-2.6f, 0.23f, 3.8f), Vec3(1.9f, 0.23f, 0.45f), _assets.wood);
    AddStaticBox("TippedChair", Vec3(2.7f, 0.18f, -3.1f), Vec3(0.65f, 0.18f, 0.34f), _assets.wood, false);

    // Eerie windows overlooking a moonless night.
    AddDecorBox("WindowNorthWest", Vec3(-7.2f, 3.0f, -11.62f), Vec3(0.72f, 0.58f, 0.05f), _assets.eerie);
    AddDecorBox("WindowNorthEast", Vec3(7.2f, 3.0f, -11.62f), Vec3(0.72f, 0.58f, 0.05f), _assets.eerie);
    AddDecorBox("WindowWest", Vec3(-16.62f, 3.0f, -5.8f), Vec3(0.05f, 0.68f, 1.05f), _assets.eerie);
    AddDecorBox("WindowEast", Vec3(16.62f, 3.0f, 5.8f), Vec3(0.05f, 0.68f, 1.05f), _assets.eerie);

    // Blood smears and claw marks are flat, non-colliding details on the room walls.
    AddDecorBox("NorthWallBloodSmear", Vec3(-7.3f, 2.0f, -11.79f), Vec3(0.42f, 0.92f, 0.018f), _assets.horrorBlood);
    AddDecorBox("StudyWallBloodSmear", Vec3(-4.84f, 1.75f, -7.2f), Vec3(0.018f, 0.78f, 0.36f), _assets.horrorBlood);
    AddDecorBox("EastCellarBloodSmear", Vec3(16.78f, 1.75f, 5.1f), Vec3(0.018f, 1.10f, 0.52f), _assets.horrorBlood);
    AddDecorBox("SouthWallBloodPool", Vec3(2.4f, 0.012f, 10.1f), Vec3(0.75f, 0.008f, 0.48f), _assets.horrorBlood);
    for (int scratch = 0; scratch < 4; scratch++) {
        const float offset = (float)scratch * 0.13f;
        AddDecorBox("WestWallClawMark" + std::to_string(scratch),
                    Vec3(-16.78f, 2.1f, -1.0f + offset),
                    Vec3(0.016f, 0.62f, 0.035f), _assets.horrorBlood,
                    Vec3(0.f, 0.f, (scratch % 2 == 0 ? -1.f : 1.f) * 7.f));
    }

    Entity* cellarRootOne = AddDecorBox("CellarHangingRoot1", Vec3(11.0f, 3.65f, 9.0f), Vec3(0.12f, 1.45f, 0.12f), _assets.mutantCarapaceByArchetype[2], Vec3(0.f, 0.f, 8.f));
    Entity* cellarRootTwo = AddDecorBox("CellarHangingRoot2", Vec3(13.0f, 3.30f, 10.8f), Vec3(0.10f, 2.05f, 0.10f), _assets.mutantCarapaceByArchetype[0], Vec3(0.f, 0.f, -11.f));
    Entity* studyRoot = AddDecorBox("StudyHangingRoot", Vec3(-14.1f, 3.72f, -8.7f), Vec3(0.11f, 1.30f, 0.11f), _assets.mutantCarapaceByArchetype[1], Vec3(0.f, 0.f, 6.f));
    _roomMotion.push_back({ cellarRootOne, Vec3(0.f, 0.f, 8.f), Vec3(0.12f, 1.45f, 0.12f), 0.4f, 0.7f, 7.f, 0.f });
    _roomMotion.push_back({ cellarRootTwo, Vec3(0.f, 0.f, -11.f), Vec3(0.10f, 2.05f, 0.10f), 2.1f, 0.52f, 5.f, 0.f });
    _roomMotion.push_back({ studyRoot, Vec3(0.f, 0.f, 6.f), Vec3(0.11f, 1.30f, 0.11f), 3.8f, 0.8f, 4.f, 0.f });

    Entity* wallHeart = _scene->AddNewEntity("CellarWallHeart");
    wallHeart->SetPosition(Vec3(16.72f, 1.55f, 8.6f));
    wallHeart->SetScale(Vec3(0.38f, 0.52f, 0.20f));
    wallHeart->AddComponent<MeshComponent>()->SetMeshResource(GetSphereMesh());
    auto* heartMaterial = wallHeart->AddComponent<MaterialComponent>();
    heartMaterial->SetMaterialResource(_assets.mutantBrain[2]);
    heartMaterial->SetCastShadows(false);
    _roomMotion.push_back({ wallHeart, Vec3(0.f), Vec3(0.38f, 0.52f, 0.20f), 1.4f, 1.25f, 0.f, 0.11f });

    // Wall candles with flickering point lights.
    const Color torchColors[4] = {
        Color(1.0f, 0.56f, 0.20f, 1.f),
        Color(0.78f, 0.20f, 0.12f, 1.f),
        Color(0.55f, 0.72f, 0.30f, 1.f),
        Color(0.78f, 0.38f, 0.22f, 1.f)
    };
    const Vec3 torchPositions[4] = {
        Vec3(-9.0f, 2.35f, -11.58f),
        Vec3(-9.0f, 2.35f, 11.58f),
        Vec3(9.0f, 2.35f, -11.58f),
        Vec3(9.0f, 2.35f, 11.58f),
    };
    for (int i = 0; i < 4; i++) {
        AddDecorBox("TorchBracket" + std::to_string(i + 1), torchPositions[i] + Vec3(0.f, -0.16f, 0.f), Vec3(0.16f, 0.22f, 0.10f), _assets.darkMetal);
        Entity* flame = AddDecorBox("TorchFlame" + std::to_string(i + 1), torchPositions[i], Vec3(0.15f, 0.22f, 0.15f), _assets.flame);
        auto* light = flame->AddComponent<LightsourceComponent>();
        light->GetLightType() = LIGHT_TYPE_POINT;
        light->GetColor() = torchColors[i];
        const float torchIntensity = i == 1 ? 1.75f : 2.35f;
        light->GetIntensity() = torchIntensity;
        light->GetRange() = 7.5f;
        light->SetCastShadows(false);
        _torches.push_back({ flame, light, (float)i * 1.71f, torchIntensity });
    }
}

void DemoLayer::CreatePlayer() {
    _player = _scene->AddNewEntity("Player");
    _player->SetPosition(Vec3(0.f, kPlayerGroundHeight, 7.0f));
    _player->SetScale(Vec3(1.f));
    auto* controller = _player->AddComponent<CharacterControllerComponent>();
    controller->SetSize(Vec2(0.32f, 0.65f));
    controller->SetCenter(Vec3(0.f));
    controller->AddToWorld();

    _playerVisual = _scene->AddNewEntity("PlayerVisual");
    _playerVisual->SetPosition(Vec3(0.f));
    _playerVisual->SetRotationEuler(Vec3(0.f, _yawDegrees, 0.f));
    _player->AddChild(_playerVisual);

    auto AddPart = [this](const char* name, const Vec3& position, const Vec3& scale, MaterialResource* material, const Vec3& rotation = Vec3(0.f)) {
        Entity* part = _scene->AddNewEntity(name);
        part->SetPosition(position);
        part->SetScale(scale);
        part->SetRotationEuler(rotation);
        part->AddComponent<MeshComponent>()->SetMeshResource(GetCubeMesh());
        auto* materialComponent = part->AddComponent<MaterialComponent>();
        materialComponent->SetMaterialResource(material);
        materialComponent->SetCastShadows(true);
        _playerVisual->AddChild(part);
        return part;
    };

    // A compact modern silhouette rather than a featureless block.
    AddPart("PlayerTorso", Vec3(0.f, 0.95f, 0.f), Vec3(0.28f, 0.50f, 0.18f), _assets.wood);
    AddPart("PlayerCoatWest", Vec3(-0.22f, 0.95f, 0.f), Vec3(0.08f, 0.46f, 0.19f), _assets.door);
    AddPart("PlayerCoatEast", Vec3(0.22f, 0.95f, 0.f), Vec3(0.08f, 0.46f, 0.19f), _assets.door);
    AddPart("PlayerHead", Vec3(0.f, 1.57f, 0.f), Vec3(0.17f, 0.19f, 0.17f), _assets.cloth)->SetActive(false);
    AddPart("PlayerHair", Vec3(0.f, 1.72f, -0.03f), Vec3(0.18f, 0.08f, 0.17f), _assets.door, Vec3(0.f, 0.f, 0.f))->SetActive(false);
    AddPart("PlayerArmWest", Vec3(-0.36f, 0.97f, 0.02f), Vec3(0.09f, 0.42f, 0.09f), _assets.door, Vec3(0.f, 0.f, 12.f));
    AddPart("PlayerArmEast", Vec3(0.36f, 0.97f, 0.02f), Vec3(0.09f, 0.42f, 0.09f), _assets.door, Vec3(0.f, 0.f, -12.f));
    AddPart("PlayerLegWest", Vec3(-0.16f, 0.39f, 0.f), Vec3(0.13f, 0.43f, 0.13f), _assets.darkMetal);
    AddPart("PlayerLegEast", Vec3(0.16f, 0.39f, 0.f), Vec3(0.13f, 0.43f, 0.13f), _assets.darkMetal);
    _cameraEntity = _scene->AddNewEntity("Camera");
    _cameraEntity->SetPosition(Vec3(0.f, kPlayerGroundHeight + kPlayerEyeHeight, 7.0f));
    _cameraEntity->SetRotation(RotationFacing(Vec3(0.f, 0.f, 1.f)));
    _camera = _cameraEntity->AddComponent<Camera>();
    _camera->SetPosition(Vec3(0.f, kPlayerGroundHeight + kPlayerEyeHeight, 7.0f));
    _camera->SetFront(Vec3(0.f, 0.f, -1.f));
    _camera->SetUp(Vec3(0.f, 1.f, 0.f));
    _camera->SetFOV(68.f);
    _camera->SetNearPlane(0.08f);
    _camera->SetFarPlane(160.f);
    _camera->SetAspectRatio((float)kScreenWidth / (float)kScreenHeight);
    _camera->GetPostEffectParams()._bloom_enabled = true;
    _camera->GetPostEffectParams()._bloom_iterations = 3;
    _camera->GetPostEffectParams()._ssao_enabled = true;
    _camera->GetPostEffectParams()._ssao_radius = 0.42f;
    _camera->GetPostEffectParams()._ssao_samples = 24;
    _camera->UpdateMatrices();
    _cameraEntity->AddComponent<AudioListenerComponent>();
    _playerVisual->SetActive(false);

    auto AddViewPart = [this](const char* name, const Vec3& position, const Vec3& scale,
                              MaterialResource* material, bool sphere) {
        Entity* part = _scene->AddNewEntity(name);
        part->SetPosition(position);
        part->SetScale(scale);
        part->AddComponent<MeshComponent>()->SetMeshResource(sphere ? GetSphereMesh() : GetCubeMesh());
        auto* materialComponent = part->AddComponent<MaterialComponent>();
        materialComponent->SetMaterialResource(material);
        materialComponent->SetCastShadows(false);
        return part;
    };
    _flashlightLens = AddViewPart("FlashlightLens", Vec3(0.f, 0.12f, 0.f),
                                  Vec3(0.018f, 0.018f, 0.018f), _assets.eerie, true);
    _firstPersonArm = AddViewPart("FirstPersonArm", Vec3(0.23f, -0.40f, -0.35f),
                                  Vec3(0.13f, 0.13f, 0.30f), _assets.door, true);
    _firstPersonFist = AddViewPart("FirstPersonFist", Vec3(0.20f, -0.34f, -0.50f),
                                   Vec3(0.13f, 0.11f, 0.13f), _assets.cloth, true);

    _exitScreamer = _scene->AddNewEntity("ExitScreamer");
    _exitScreamer->SetPosition(Vec3(0.f));
    _exitScreamer->SetActive(false);
    auto AddScreamerPart = [this](const char* name, const Vec3& position, const Vec3& scale,
                                  MaterialResource* material) {
        Entity* part = _scene->AddNewEntity(name);
        part->SetPosition(position);
        part->SetScale(scale);
        part->AddComponent<MeshComponent>()->SetMeshResource(GetSphereMesh());
        auto* materialComponent = part->AddComponent<MaterialComponent>();
        materialComponent->SetMaterialResource(material);
        materialComponent->SetCastShadows(false);
        _exitScreamer->AddChild(part, false);
    };
    std::mt19937 scareRandomEngine{ std::random_device{}() };
    auto scareRange = [&scareRandomEngine](float minimum, float maximum) {
        std::uniform_real_distribution<float> distribution(minimum, maximum);
        return distribution(scareRandomEngine);
    };
    std::uniform_int_distribution<int> scareShapeDistribution(0, 3);
    std::uniform_int_distribution<int> scareEyeDistribution(1, 3);
    std::uniform_int_distribution<int> scareGrowthDistribution(2, 5);
    const int scareShape = scareShapeDistribution(scareRandomEngine);
    const Vec3 scareHeadScale = scareShape == 0 ? Vec3(0.75f, 1.22f, 0.44f)
        : (scareShape == 1 ? Vec3(1.18f, 0.80f, 0.50f)
        : (scareShape == 2 ? Vec3(0.94f, 0.95f, 0.62f) : Vec3(0.82f, 1.08f, 0.38f)));
    const float scareEyeY = scareShape == 0 ? 0.30f : (scareShape == 1 ? 0.12f : 0.20f);
    const float scareEyeSpacing = scareShape == 1 ? 0.38f : 0.29f;
    const int scareEyeCount = scareEyeDistribution(scareRandomEngine);
    AddScreamerPart("ExitScreamerHead", Vec3(0.f, 0.f, 0.f),
                    scareHeadScale, _assets.mutantCarapace);
    for (int eye = 0; eye < scareEyeCount; eye++) {
        const float eyeX = scareEyeCount == 1 ? scareRange(-0.16f, 0.16f)
            : -scareEyeSpacing + (2.f * scareEyeSpacing * eye / (scareEyeCount - 1));
        const float eyeY = scareEyeY + (scareEyeCount == 3 && eye == 1 ? 0.27f : 0.f);
        const float eyeScale = scareEyeCount == 3 && eye == 1 ? 0.70f : 1.f;
        const std::string eyeSuffix = std::to_string(eye + 1);
        const std::string socketName = "ExitScreamerSocket" + eyeSuffix;
        const std::string eyeName = "ExitScreamerEye" + eyeSuffix;
        AddScreamerPart(socketName.c_str(), Vec3(eyeX, eyeY, 0.39f),
                        Vec3(0.27f * eyeScale, 0.30f * eyeScale, 0.10f), _assets.darkMetal);
        AddScreamerPart(eyeName.c_str(), Vec3(eyeX, eyeY, 0.47f),
                        Vec3(0.15f * eyeScale, 0.18f * eyeScale, 0.09f), _assets.eyes);
    }
    const float mouthWidth = scareShape == 1 ? 0.68f : (scareShape == 0 ? 0.28f : 0.42f);
    AddScreamerPart("ExitScreamerMouth", Vec3(0.f, -0.29f, 0.42f),
                    Vec3(mouthWidth, scareRange(0.20f, 0.34f), 0.12f), _assets.darkMetal);
    AddScreamerPart("ExitScreamerJaw", Vec3(0.f, -0.43f, 0.36f),
                    Vec3(mouthWidth + 0.05f, scareRange(0.10f, 0.19f), 0.13f), _assets.brain);
    const int toothCount = scareGrowthDistribution(scareRandomEngine) + 1;
    for (int tooth = 0; tooth < toothCount; tooth++) {
        const float toothX = -mouthWidth * 0.72f + mouthWidth * 1.44f * tooth / (toothCount - 1);
        const std::string toothName = "ExitScreamerTooth" + std::to_string(tooth);
        AddScreamerPart(toothName.c_str(),
                        Vec3(toothX, -0.31f + scareRange(-0.035f, 0.035f), 0.52f),
                        Vec3(scareRange(0.035f, 0.075f), scareRange(0.08f, 0.17f), 0.055f),
                        _assets.cloth);
    }
    const int growthCount = scareGrowthDistribution(scareRandomEngine);
    for (int growth = 0; growth < growthCount; growth++) {
        const float side = growth % 2 == 0 ? -1.f : 1.f;
        const bool horn = growth < 2;
        const std::string growthName = horn
            ? "ExitScreamerHorn" + std::to_string(growth)
            : "ExitScreamerTendril" + std::to_string(growth);
        const float length = scareRange(horn ? 0.22f : 0.30f, horn ? 0.48f : 0.62f);
        const float x = side * scareRange(0.42f, 0.75f);
        const float y = horn ? scareRange(0.56f, 0.78f) : scareRange(-0.12f, 0.24f);
        AddScreamerPart(growthName.c_str(), Vec3(x, y, scareRange(0.02f, 0.20f)),
                        Vec3(scareRange(0.07f, 0.15f), length, scareRange(0.07f, 0.16f)),
                        horn ? _assets.brain : _assets.mutantBase);
    }
    AddScreamerPart("ExitScreamerBrowLeft", Vec3(-scareEyeSpacing, scareEyeY + 0.23f, 0.36f),
                    Vec3(scareEyeSpacing * 1.05f, 0.09f, 0.12f), _assets.brain);
    AddScreamerPart("ExitScreamerBrowRight",
                    Vec3(scareEyeSpacing * scareRange(0.82f, 1.18f), scareEyeY + 0.23f, 0.36f),
                    Vec3(scareEyeSpacing * scareRange(0.85f, 1.20f), 0.09f, 0.12f), _assets.brain);

    _flashlight = _scene->AddNewEntity("Flashlight");
    _flashlight->SetPosition(Vec3(0.f, kPlayerGroundHeight + kPlayerEyeHeight + 0.12f, 7.0f));
    _flashlightLight = _flashlight->AddComponent<LightsourceComponent>();
    _flashlightLight->SetDirection(Vec3(0.f, 0.f, -1.f));
    _flashlightLight->GetLightType() = LIGHT_TYPE_SPOT;
    _flashlightLight->GetColor() = Color(1.0f, 0.92f, 0.78f, 1.f);
    _flashlightLight->GetIntensity() = 9.0f;
    _flashlightLight->GetSpotAngle() = 30.f;
    _flashlightLight->GetRange() = 19.f;
    _flashlightLight->SetCastShadows(false);
    if (_flashlightLens)
        _flashlightLens->SetActive(_flashlightEnabled);

    Entity* moon = _scene->AddNewEntity("Moon");
    moon->SetPosition(Vec3(6.0f, 24.0f, -18.0f));
    moon->SetRotationEuler(Vec3(38.f, 210.f, 0.f));
    auto* moonLight = moon->AddComponent<LightsourceComponent>();
    moonLight->GetLightType() = LIGHT_TYPE_DIRECTIONAL;
    moonLight->GetColor() = Color(0.45f, 0.58f, 0.78f, 1.f);
    moonLight->GetIntensity() = 0.22f;
    moonLight->SetCastShadows(false);
}

void DemoLayer::CreateArtifacts() {
    const char* names[5] = {
        "BROKEN LOCKET", "BONE KEY", "GLASS EYE", "ASHEN IDOL", "CURSED BELL"
    };
    const Vec3 spawnChoices[5][3] = {
        { Vec3(-15.3f, 0.82f, 9.4f), Vec3(-15.5f, 0.82f, 2.8f), Vec3(-16.1f, 0.82f, 6.1f) },
        { Vec3(-8.3f, 0.82f, -8.7f), Vec3(-8.6f, 0.82f, -5.2f), Vec3(-13.8f, 0.82f, -8.8f) },
        { Vec3(8.3f, 0.82f, -8.7f), Vec3(8.4f, 0.82f, -5.1f), Vec3(13.8f, 0.82f, -8.9f) },
        { Vec3(7.2f, 0.82f, 8.4f), Vec3(6.6f, 0.82f, 6.1f), Vec3(15.7f, 0.82f, 4.7f) },
        { Vec3(-2.2f, 0.82f, -2.5f), Vec3(2.4f, 0.82f, -3.4f), Vec3(2.5f, 0.82f, 3.2f) },
    };
    std::mt19937 randomEngine{ std::random_device{}() };
    std::uniform_int_distribution<int> chooseSpawn(0, 2);
    Vec3 bases[5];
    for (int region = 0; region < 5; region++)
        bases[region] = spawnChoices[region][chooseSpawn(randomEngine)];

    for (int i = 0; i < 5; i++) {
        Entity* artifact = _scene->AddNewEntity("CursedArtifact" + std::to_string(i + 1));
        artifact->SetPosition(bases[i]);
        artifact->SetScale(Vec3(0.34f));
        auto AddPart = [this, artifact, i](const char* suffix, const Vec3& position, const Vec3& scale,
                                           MaterialResource* material, bool sphere = true) {
            Entity* part = _scene->AddNewEntity("Relic" + std::to_string(i + 1) + suffix);
            part->SetPosition(position);
            part->SetScale(scale);
            part->AddComponent<MeshComponent>()->SetMeshResource(sphere ? GetSphereMesh() : GetCubeMesh());
            auto* materialComponent = part->AddComponent<MaterialComponent>();
            materialComponent->SetMaterialResource(material);
            materialComponent->SetCastShadows(false);
            artifact->AddChild(part, false);
            return part;
        };

        switch (i) {
        case 0:
            AddPart("LocketBody", Vec3(0.f), Vec3(0.86f, 1.0f, 0.34f), _assets.artifact);
            AddPart("LocketFrame", Vec3(0.f, 0.f, 0.30f), Vec3(0.62f, 0.73f, 0.16f), _assets.darkMetal);
            AddPart("LocketPhoto", Vec3(0.f, 0.f, 0.39f), Vec3(0.41f, 0.52f, 0.08f), _assets.eyes);
            AddPart("LocketLoop", Vec3(0.f, 0.94f, 0.f), Vec3(0.24f, 0.22f, 0.24f), _assets.darkMetal);
            break;
        case 1:
            AddPart("KeyShaft", Vec3(0.f, -0.12f, 0.f), Vec3(0.16f, 1.28f, 0.16f), _assets.artifact, false);
            AddPart("KeyStem", Vec3(0.f, -0.59f, 0.f), Vec3(0.18f, 0.22f, 0.18f), _assets.artifact, false);
            AddPart("KeyToothA", Vec3(0.13f, -0.53f, 0.f), Vec3(0.16f, 0.14f, 0.16f), _assets.darkMetal, false);
            AddPart("KeyToothB", Vec3(-0.13f, -0.38f, 0.f), Vec3(0.15f, 0.14f, 0.16f), _assets.darkMetal, false);
            AddPart("KeyBowTop", Vec3(0.f, 0.64f, 0.f), Vec3(0.52f, 0.16f, 0.16f), _assets.darkMetal, false);
            AddPart("KeyBowSideA", Vec3(-0.23f, 0.44f, 0.f), Vec3(0.15f, 0.38f, 0.16f), _assets.darkMetal, false);
            AddPart("KeyBowSideB", Vec3(0.23f, 0.44f, 0.f), Vec3(0.15f, 0.38f, 0.16f), _assets.darkMetal, false);
            AddPart("KeyBowBottom", Vec3(0.f, 0.26f, 0.f), Vec3(0.52f, 0.15f, 0.16f), _assets.darkMetal, false);
            break;
        case 2:
            AddPart("GlassEyeWhite", Vec3(0.f), Vec3(1.0f, 0.72f, 0.72f), _assets.cloth);
            AddPart("GlassEyeIris", Vec3(0.f, 0.f, 0.59f), Vec3(0.42f, 0.42f, 0.19f), _assets.eyes);
            AddPart("GlassEyePupil", Vec3(0.f, 0.f, 0.73f), Vec3(0.19f, 0.22f, 0.10f), _assets.darkMetal);
            AddPart("GlassEyeGlint", Vec3(-0.12f, 0.13f, 0.80f), Vec3(0.10f, 0.10f, 0.05f), _assets.artifact);
            break;
        case 3:
            AddPart("IdolBase", Vec3(0.f, -0.45f, 0.f), Vec3(0.95f, 0.32f, 0.78f), _assets.darkMetal, false);
            AddPart("IdolBody", Vec3(0.f, 0.02f, 0.f), Vec3(0.72f, 0.88f, 0.55f), _assets.artifact, false);
            AddPart("IdolHead", Vec3(0.f, 0.68f, 0.f), Vec3(0.58f, 0.56f, 0.52f), _assets.brain);
            AddPart("IdolEyeLeft", Vec3(-0.18f, 0.75f, 0.42f), Vec3(0.12f, 0.13f, 0.08f), _assets.eyes);
            AddPart("IdolEyeRight", Vec3(0.18f, 0.75f, 0.42f), Vec3(0.12f, 0.13f, 0.08f), _assets.eyes);
            AddPart("IdolCrown", Vec3(0.f, 1.08f, 0.f), Vec3(0.72f, 0.14f, 0.60f), _assets.darkMetal, false);
            break;
        case 4:
            AddPart("BellCup", Vec3(0.f, -0.12f, 0.f), Vec3(0.86f, 0.73f, 0.86f), _assets.artifact);
            AddPart("BellLip", Vec3(0.f, -0.53f, 0.f), Vec3(1.0f, 0.16f, 1.0f), _assets.darkMetal, false);
            AddPart("BellCap", Vec3(0.f, 0.53f, 0.f), Vec3(0.34f, 0.20f, 0.34f), _assets.darkMetal);
            AddPart("BellHandle", Vec3(0.f, 0.78f, 0.f), Vec3(0.15f, 0.32f, 0.15f), _assets.artifact, false);
            AddPart("BellClapper", Vec3(0.f, -0.59f, 0.f), Vec3(0.18f, 0.18f, 0.18f), _assets.brain);
            break;
        }
        _artifacts.push_back({ artifact, bases[i], false, names[i] });
    }
}

void DemoLayer::CreatePickups() {
    struct PickupSpawn {
        Vec3 position;
        PickupType type;
    };
    std::vector<PickupSpawn> candidates = {
        { Vec3(-15.2f, 0.62f, 4.8f), PickupType::Health },
        { Vec3(-15.7f, 0.62f, 11.0f), PickupType::Stamina },
        { Vec3(-9.1f, 0.62f, 3.8f), PickupType::Health },
        { Vec3(-8.1f, 0.62f, -10.0f), PickupType::Stamina },
        { Vec3(-14.1f, 0.62f, -4.8f), PickupType::Health },
        { Vec3(-8.2f, 0.62f, -5.3f), PickupType::Stamina },
        { Vec3(8.1f, 0.62f, -10.0f), PickupType::Health },
        { Vec3(14.1f, 0.62f, -4.8f), PickupType::Stamina },
        { Vec3(8.2f, 0.62f, -5.3f), PickupType::Health },
        { Vec3(7.0f, 0.62f, 6.4f), PickupType::Stamina },
        { Vec3(15.7f, 0.62f, 4.5f), PickupType::Health },
        { Vec3(7.0f, 0.62f, 8.0f), PickupType::Stamina },
        { Vec3(-2.6f, 0.62f, -3.2f), PickupType::Health },
        { Vec3(2.6f, 0.62f, -3.2f), PickupType::Stamina },
        { Vec3(2.5f, 0.62f, 3.0f), PickupType::Health },
        { Vec3(-2.5f, 0.62f, 3.0f), PickupType::Stamina },
    };
    std::mt19937 randomEngine{ std::random_device{}() };
    std::shuffle(candidates.begin(), candidates.end(), randomEngine);

    std::vector<PickupSpawn> selectedSpawns;
    std::vector<Vec3> selectedPositions;
    uint32 healthPickups = 0;
    uint32 staminaPickups = 0;
    auto selectSpawns = [&](float artifactSpacing, float pickupSpacing) {
        for (const PickupSpawn& candidate : candidates) {
            if (selectedSpawns.size() == 6)
                return;
            const uint32 typeCount = candidate.type == PickupType::Health ? healthPickups : staminaPickups;
            if (typeCount >= 3)
                continue;

            bool overlapsArtifact = false;
            for (const Artifact& artifact : _artifacts) {
                const Vec3 delta(candidate.position.x - artifact.basePosition.x, 0.f,
                                 candidate.position.z - artifact.basePosition.z);
                overlapsArtifact = overlapsArtifact || delta.Length() < artifactSpacing;
            }
            bool overlapsPickup = false;
            for (const Vec3& position : selectedPositions) {
                const Vec3 delta(candidate.position.x - position.x, 0.f,
                                 candidate.position.z - position.z);
                overlapsPickup = overlapsPickup || delta.Length() < pickupSpacing;
            }
            if (overlapsArtifact || overlapsPickup)
                continue;

            selectedSpawns.push_back(candidate);
            selectedPositions.push_back(candidate.position);
            if (candidate.type == PickupType::Health)
                healthPickups++;
            else
                staminaPickups++;
        }
    };
    selectSpawns(2.15f, 3.0f);
    if (selectedSpawns.size() < 6)
        selectSpawns(1.4f, 1.8f);
    if (selectedSpawns.size() < 6)
        selectSpawns(0.8f, 1.0f);

    for (const PickupSpawn& candidate : selectedSpawns) {
        const uint32 index = static_cast<uint32>(_pickups.size());
        Entity* root = _scene->AddNewEntity(
            candidate.type == PickupType::Health ? "BloodTonicPickup" : "GreenSaltsPickup");
        root->SetPosition(candidate.position);
        root->SetScale(Vec3(1.f));
        MaterialResource* material = candidate.type == PickupType::Health
            ? _assets.healthPickup : _assets.staminaPickup;
        auto AddPart = [this, root, material, index](const char* suffix, const Vec3& position,
                                                     const Vec3& scale, MaterialResource* partMaterial = nullptr,
                                                     bool sphere = true) {
            Entity* part = _scene->AddNewEntity("Pickup" + std::to_string(index) + suffix);
            part->SetPosition(position);
            part->SetScale(scale);
            part->AddComponent<MeshComponent>()->SetMeshResource(sphere ? GetSphereMesh() : GetCubeMesh());
            auto* materialComponent = part->AddComponent<MaterialComponent>();
            materialComponent->SetMaterialResource(partMaterial ? partMaterial : material);
            materialComponent->SetCastShadows(false);
            root->AddChild(part, false);
        };

        if (candidate.type == PickupType::Health) {
            AddPart("Vial", Vec3(0.f), Vec3(0.28f, 0.42f, 0.20f));
            AddPart("VialNeck", Vec3(0.f, 0.44f, 0.f), Vec3(0.15f, 0.13f, 0.15f), _assets.darkMetal);
            AddPart("MedicalCrossVertical", Vec3(0.f, 0.f, 0.205f), Vec3(0.045f, 0.18f, 0.035f), _assets.mutantBone, false);
            AddPart("MedicalCrossHorizontal", Vec3(0.f, 0.f, 0.21f), Vec3(0.15f, 0.045f, 0.035f), _assets.mutantBone, false);
        } else {
            AddPart("SaltPouch", Vec3(0.f), Vec3(0.31f, 0.36f, 0.20f));
            AddPart("PouchSeal", Vec3(0.f, 0.28f, 0.f), Vec3(0.18f, 0.08f, 0.17f), _assets.darkMetal, false);
            AddPart("SaltMark", Vec3(0.f, 0.f, 0.205f), Vec3(0.045f, 0.21f, 0.035f), _assets.mutantBone, false);
        }

        const float phase = Hash2D(index, candidate.type == PickupType::Health ? 1u : 2u, 83719u) * 2.f * kPi;
        _pickups.push_back({ root, candidate.position, candidate.type, phase, false });
    }

    if (_pickups.size() != 6)
        Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: only " << static_cast<uint32>(_pickups.size())
            << " of 6 resource pickups fit the randomized layout\n";
}

void DemoLayer::CreateMonsters() {
    struct MonsterSpawn {
        const char* name;
        Vec3 position;
    };
    MonsterSpawn spawns[3] = {
        { "The Rook", Vec3(0.f) },
        { "The Pale Librarian", Vec3(0.f) },
        { "The Cellar Choir", Vec3(0.f) },
    };
    std::mt19937 randomEngine{ std::random_device{}() };
    auto randomRange = [&randomEngine](float minimum, float maximum) {
        std::uniform_real_distribution<float> distribution(minimum, maximum);
        return distribution(randomEngine);
    };
    std::vector<Vec3> spawnPoints = {
        Vec3(-15.3f, 0.95f, 2.6f), Vec3(-15.4f, 0.95f, 10.6f),
        Vec3(-8.5f, 0.95f, -8.7f), Vec3(-13.6f, 0.95f, -4.7f),
        Vec3(8.5f, 0.95f, -8.7f), Vec3(13.6f, 0.95f, -4.7f),
        Vec3(7.0f, 0.95f, 6.3f), Vec3(15.6f, 0.95f, 4.4f),
        Vec3(-2.6f, 0.95f, -3.4f), Vec3(2.6f, 0.95f, 3.2f),
    };
    std::shuffle(spawnPoints.begin(), spawnPoints.end(), randomEngine);
    uint32 nextSpawn = 0;
    for (MonsterSpawn& spawn : spawns) {
        bool foundSpawn = false;
        while (nextSpawn < spawnPoints.size() && !foundSpawn) {
            const Vec3 candidate = spawnPoints[nextSpawn++];
            foundSpawn = true;
            for (const MonsterSpawn& chosen : spawns) {
                if (&chosen == &spawn)
                    break;
                if ((candidate - chosen.position).Length() < 6.f) {
                    foundSpawn = false;
                    break;
                }
            }
            if (foundSpawn)
                spawn.position = candidate;
        }
        if (!foundSpawn) {
            Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: using fallback monster spawn point\n";
            spawn.position = spawnPoints[(nextSpawn - 1) % spawnPoints.size()];
        }
    }
    std::vector<int> archetypes = { 0, 1, 2 };
    std::shuffle(archetypes.begin(), archetypes.end(), randomEngine);

    for (uint32 i = 0; i < 3; i++) {
        const int archetype = archetypes[i];
        MaterialResource* hideMaterial = _assets.mutantHide[archetype];
        MaterialResource* carapaceMaterial = _assets.mutantCarapaceByArchetype[archetype];
        MaterialResource* brainMaterial = _assets.mutantBrain[archetype];
        MaterialResource* eyeMaterial = _assets.mutantEyes[archetype];
        const Vec3 profileBodyScale[3] = {
            Vec3(0.30f, 1.08f, 0.33f),
            Vec3(0.64f, 0.78f, 0.48f),
            Vec3(0.44f, 0.40f, 0.84f)
        };
        const Vec3 bodyScale(profileBodyScale[archetype].x * randomRange(0.78f, 1.22f),
                             profileBodyScale[archetype].y * randomRange(0.84f, 1.16f),
                             profileBodyScale[archetype].z * randomRange(0.82f, 1.18f));
        const float bodyY = archetype == 0 ? 0.18f : (archetype == 1 ? 0.02f : 0.0f);
        const float headWidth = (archetype == 0 ? 0.31f : (archetype == 1 ? 0.48f : 0.34f))
            * randomRange(0.94f, 1.06f);
        const float headHeight = (archetype == 0 ? 0.43f : (archetype == 1 ? 0.39f : 0.32f))
            * randomRange(0.95f, 1.05f);
        const float headDepth = (archetype == 0 ? 0.36f : (archetype == 1 ? 0.43f : 0.42f))
            * randomRange(0.95f, 1.05f);
        const Vec3 stalkerGrowthPosition(-bodyScale.x * 0.72f, bodyY + bodyScale.y * 0.34f, bodyScale.z * 0.42f);
        const Vec3 stalkerGrowthScale(randomRange(0.13f, 0.19f), randomRange(0.24f, 0.36f), randomRange(0.12f, 0.17f));
        Entity* monster = _scene->AddNewEntity(spawns[i].name);
        monster->SetPosition(spawns[i].position);
        _lastSafeMonsterPos[i] = spawns[i].position;
        monster->SetScale(Vec3(1.f));
        auto* controller = monster->AddComponent<CharacterControllerComponent>();
        const Vec2 colliderSize[3] = { Vec2(0.27f, 1.38f), Vec2(0.39f, 0.68f), Vec2(0.30f, 0.72f) };
        controller->SetSize(colliderSize[archetype]);
        controller->SetCenter(Vec3(0.f));
        controller->AddToWorld();
        auto* vocalSource = monster->AddComponent<AudioSourceComponent>();
        vocalSource->SetVolume(0.95f);

        Entity* visual = _scene->AddNewEntity(std::string(spawns[i].name) + "Visual");
        visual->SetPosition(Vec3(0.f));
        monster->AddChild(visual, false);

        auto AddPart = [this, visual, &spawns, i](const char* suffix, const Vec3& position, const Vec3& scale, MaterialResource* material, bool sphere = true, const Vec3& rotation = Vec3(0.f), Entity* parent = nullptr) {
            Entity* part = _scene->AddNewEntity(std::string(spawns[i].name) + suffix);
            part->SetPosition(position);
            part->SetScale(scale);
            part->SetRotationEuler(rotation);
            part->AddComponent<MeshComponent>()->SetMeshResource(sphere ? GetSphereMesh() : GetCubeMesh());
            auto* materialComponent = part->AddComponent<MaterialComponent>();
            materialComponent->SetMaterialResource(material);
            materialComponent->SetCastShadows(true);
            (parent ? parent : visual)->AddChild(part, false);
            return part;
        };

        Entity* body = AddPart("TornMutantTorso", Vec3(0.f, bodyY, 0.f), bodyScale, hideMaterial);
        std::vector<Entity*> hitRegions = { body };
        if (archetype == 0) {
            AddPart("SplitCrown", Vec3(0.f, bodyY + bodyScale.y * 0.91f, 0.f),
                    Vec3(bodyScale.x * 0.50f, 0.10f, bodyScale.z * 0.50f), carapaceMaterial);
            AddPart("LowerSpur", Vec3(0.f, bodyY - bodyScale.y * 0.87f, 0.f),
                    Vec3(bodyScale.x * 0.38f, 0.12f, bodyScale.z * 0.38f), carapaceMaterial);
        } else if (archetype == 2) {
            for (int segment = 0; segment < 2; segment++) {
                const float z = segment == 0 ? -bodyScale.z * 0.54f : bodyScale.z * 0.48f;
                const float taper = segment == 0 ? 0.78f : 0.62f;
                Entity* section = AddPart(segment == 0 ? "CrawlerRearSegment" : "CrawlerNeckSegment",
                    Vec3(0.f, bodyY + 0.015f, z),
                    Vec3(bodyScale.x * taper, bodyScale.y * (segment == 0 ? 0.82f : 0.72f),
                         bodyScale.z * (segment == 0 ? 0.40f : 0.36f)),
                    segment == 0 ? hideMaterial : carapaceMaterial);
                hitRegions.push_back(section);
            }
            for (int plate = 0; plate < 3; plate++) {
                const std::string plateName = "CrawlerDorsalPlate" + std::to_string(plate);
                const float z = -bodyScale.z * 0.60f + plate * bodyScale.z * 0.60f;
                AddPart(plateName.c_str(),
                        Vec3(0.f, bodyY + bodyScale.y * 0.76f, z),
                        Vec3(bodyScale.x * 0.54f, 0.10f, 0.14f), carapaceMaterial);
            }
        }

        Entity* growth = nullptr;
        if (archetype == 0) {
            growth = AddPart("StalkerShoulderParasite", stalkerGrowthPosition, stalkerGrowthScale, brainMaterial);
            hitRegions.push_back(growth);
        }

        Entity* head = _scene->AddNewEntity(std::string(spawns[i].name) + "MutantHeadPivot");
        const Vec3 headPosition = archetype == 0
            ? Vec3(randomRange(-0.06f, 0.06f), bodyY + bodyScale.y * 0.92f, 0.08f)
            : (archetype == 1
                ? Vec3(0.f, bodyY + bodyScale.y * 0.38f, bodyScale.z * 0.84f)
                : Vec3(0.f, bodyY + bodyScale.y * 0.48f, bodyScale.z * 0.86f));
        head->SetPosition(headPosition);
        visual->AddChild(head, false);
        Entity* skull = AddPart("SplitSkull", Vec3(0.f), Vec3(headWidth, headHeight, headDepth), hideMaterial, true, Vec3(0.f), head);
        hitRegions.push_back(skull);
        const float muzzleWidth = headWidth * (archetype == 1 ? 0.78f : 0.82f);
        const float muzzleHeight = headHeight * (archetype == 1 ? 0.54f : 0.48f);
        Entity* muzzle = AddPart("RaggedMuzzle", Vec3(0.f, -headHeight * 0.28f, headDepth + 0.005f),
                Vec3(muzzleWidth * 1.12f, muzzleHeight * 0.88f, 0.09f),
                hideMaterial, true, Vec3(0.f), head);
        hitRegions.push_back(muzzle);
        Entity* mouth = AddPart("OpenRictus", Vec3(0.f, -headHeight * 0.29f, headDepth + 0.075f),
                Vec3(muzzleWidth, muzzleHeight, 0.055f),
                _assets.mutantMouth[archetype], true, Vec3(0.f), head);
        hitRegions.push_back(mouth);
        Entity* jaw = AddPart("UnhingedJaw", Vec3(0.f, -headHeight * 0.56f, headDepth + 0.06f),
                Vec3(muzzleWidth * 0.88f, headHeight * 0.27f, 0.11f),
                hideMaterial, true, Vec3(0.f), head);
        hitRegions.push_back(jaw);
        for (int toothIndex = 0; toothIndex < 6; toothIndex++) {
            const float x = -muzzleWidth * 0.70f + muzzleWidth * 1.40f * toothIndex / 5.f;
            const bool upperRow = toothIndex % 2 == 0;
            const float toothHeight = headHeight * (toothIndex == 0 || toothIndex == 5 ? 0.22f : 0.15f);
            const Vec3 toothPosition(x, upperRow ? -headHeight * 0.24f : -headHeight * 0.48f,
                                     headDepth + 0.15f);
            const Vec3 toothScale(headWidth * 0.075f, toothHeight, 0.035f);
            const float lean = x < 0.f ? 12.f : -12.f;
            const Vec3 toothRotation(0.f, 0.f, lean);
            Entity* tooth = AddPart(("RippingTooth" + std::to_string(toothIndex)).c_str(),
                                    toothPosition, toothScale, _assets.mutantBone, true, toothRotation, head);
            hitRegions.push_back(tooth);
        }
        const int eyeCount = archetype == 2 ? 3 : 2;
        for (int eye = 0; eye < eyeCount; eye++) {
            const bool thirdEye = eyeCount == 3 && eye == 2;
            const float eyeX = thirdEye ? 0.f : (eye == 0 ? -headWidth * 0.36f : headWidth * 0.36f);
            const float eyeY = thirdEye ? headHeight * 0.47f : headHeight * 0.19f;
            const float eyeScale = thirdEye ? 0.72f : 1.f;
            const Vec3 eyePosition(eyeX, eyeY, headDepth + 0.04f);
            const Vec3 socketScale(headWidth * 0.24f * eyeScale, headHeight * 0.24f * eyeScale, 0.075f);
            AddPart(("DeepEyeSocket" + std::to_string(eye + 1)).c_str(),
                    eyePosition, socketScale, _assets.mutantMouth[archetype], true,
                    Vec3(0.f, 0.f, eyeX < 0.f ? -12.f : 12.f), head);
            const std::string eyeName = "MutantEye" + std::to_string(eye + 1);
            AddPart(eyeName.c_str(), eyePosition + Vec3(0.f, 0.f, 0.10f),
                    Vec3(headWidth * 0.135f * eyeScale, headHeight * 0.14f * eyeScale, 0.045f),
                    eyeMaterial, true, Vec3(0.f, 0.f, eyeX < 0.f ? -12.f : 12.f), head);
            const float browSide = eyeX < 0.f ? -1.f : 1.f;
            AddPart(("KnittedBrow" + std::to_string(eye + 1)).c_str(),
                    eyePosition + Vec3(0.f, headHeight * 0.23f, -0.01f),
                    Vec3(headWidth * 0.29f * eyeScale, headHeight * 0.09f, 0.08f), carapaceMaterial, true,
                    Vec3(0.f, 0.f, -browSide * 22.f), head);
        }

        std::vector<Entity*> writhingParts;
        std::vector<Vec3> writhingBaseRotations;
        if (archetype == 0) {
            AddPart("StalkerNeck", Vec3(0.f, bodyY + bodyScale.y * 0.72f, 0.02f),
                    Vec3(0.15f, 0.34f, 0.15f), carapaceMaterial);
            for (int sideIndex = 0; sideIndex < 2; sideIndex++) {
                const float side = sideIndex == 0 ? -1.f : 1.f;
                const Vec3 rotation(0.f, 0.f, side * randomRange(18.f, 28.f));
                Entity* limb = AddPart(sideIndex == 0 ? "StalkerArmLeft" : "StalkerArmRight",
                    Vec3(side * 0.29f, bodyY - 0.03f, 0.16f), Vec3(0.095f, 0.72f, 0.10f),
                    hideMaterial, true, rotation);
                hitRegions.push_back(limb);
                writhingParts.push_back(limb);
                writhingBaseRotations.push_back(rotation);
                Entity* claw = AddPart(sideIndex == 0 ? "StalkerClawLeft" : "StalkerClawRight",
                    Vec3(side * 0.38f, bodyY - 0.53f, 0.26f), Vec3(0.035f, 0.23f, 0.045f),
                    _assets.darkMetal, true, Vec3(side * 18.f, 0.f, side * 8.f));
                hitRegions.push_back(claw);
            }
        } else if (archetype == 1) {
            for (int plate = 0; plate < 3; plate++) {
                const float side = (float)plate - 1.f;
                AddPart(("BruteBreastplate" + std::to_string(plate)).c_str(),
                    Vec3(side * bodyScale.x * 0.43f, bodyY + bodyScale.y * 0.22f,
                         bodyScale.z * 0.82f),
                    Vec3(bodyScale.x * 0.34f, bodyScale.y * 0.23f, 0.075f),
                    _assets.mutantBone, false, Vec3(0.f, 0.f, -side * 8.f));
            }
            for (int sideIndex = 0; sideIndex < 2; sideIndex++) {
                const float side = sideIndex == 0 ? -1.f : 1.f;
                const Vec3 shoulderPosition(side * bodyScale.x * 0.78f, bodyY + 0.12f, bodyScale.z * 0.45f);
                const Vec3 shoulderScale(0.28f, 0.25f, 0.30f);
                AddPart(sideIndex == 0 ? "BruteShoulderLeft" : "BruteShoulderRight",
                        shoulderPosition, shoulderScale, hideMaterial);
                const Vec3 rotation(0.f, 0.f, side * randomRange(42.f, 58.f));
                Entity* arm = AddPart(sideIndex == 0 ? "BruteArmLeft" : "BruteArmRight",
                    Vec3(side * bodyScale.x * 0.88f, bodyY - 0.22f, bodyScale.z * 0.72f),
                    Vec3(0.16f, 0.56f, 0.16f), hideMaterial, true, rotation);
                hitRegions.push_back(arm);
                writhingParts.push_back(arm);
                writhingBaseRotations.push_back(rotation);
                Entity* hook = AddPart(sideIndex == 0 ? "BruteHookLeft" : "BruteHookRight",
                    Vec3(side * bodyScale.x * 1.02f, bodyY - 0.60f, bodyScale.z * 1.04f),
                    Vec3(0.07f, 0.25f, 0.08f), _assets.darkMetal, true,
                    Vec3(18.f, 0.f, -side * 22.f));
                hitRegions.push_back(hook);
            }
            for (int spike = 0; spike < 3; spike++) {
                const float z = -bodyScale.z * 0.56f + spike * bodyScale.z * 0.56f;
                AddPart(("BruteDorsalSpine" + std::to_string(spike)).c_str(),
                    Vec3(0.f, bodyY + bodyScale.y * 0.80f, z),
                    Vec3(0.075f, 0.25f, 0.08f),
                    _assets.darkMetal, true, Vec3(-18.f, 0.f, 0.f));
            }
        } else {
            for (int leg = 0; leg < 6; leg++) {
                const float side = leg % 2 == 0 ? -1.f : 1.f;
                const int pair = leg / 2;
                const float z = bodyScale.z * (0.50f - pair * 0.48f);
                const Vec3 rotation((pair - 1) * 7.f, 0.f, side * (34.f + pair * 5.f));
                const std::string limbName = (side < 0.f ? "CrawlerLegL" : "CrawlerLegR")
                    + std::to_string(pair);
                Entity* limb = AddPart(limbName.c_str(),
                    Vec3(side * bodyScale.x * 0.68f, -0.24f, z),
                    Vec3(0.085f, 0.58f + pair * 0.06f, 0.09f),
                    hideMaterial, true, rotation);
                hitRegions.push_back(limb);
                writhingParts.push_back(limb);
                writhingBaseRotations.push_back(rotation);
                const std::string clawName = (side < 0.f ? "CrawlerClawL" : "CrawlerClawR")
                    + std::to_string(pair);
                Entity* claw = AddPart(clawName.c_str(),
                    Vec3(side * bodyScale.x * 0.96f, -0.52f, z + bodyScale.z * 0.08f),
                    Vec3(0.035f, 0.18f, 0.04f), _assets.darkMetal, true,
                    Vec3(0.f, 0.f, -side * 12.f));
                hitRegions.push_back(claw);
            }
            for (int sideIndex = 0; sideIndex < 2; sideIndex++) {
                const float side = sideIndex == 0 ? -1.f : 1.f;
                const Vec3 rotation(-12.f, 0.f, side * 20.f);
                Entity* feeler = AddPart(sideIndex == 0 ? "CrawlerFeelerLeft" : "CrawlerFeelerRight",
                    Vec3(side * 0.12f, bodyY + 0.35f, bodyScale.z * 0.72f),
                    Vec3(0.055f, 0.38f, 0.055f), carapaceMaterial, true, rotation);
                writhingParts.push_back(feeler);
                writhingBaseRotations.push_back(rotation);
            }
        }

        Monster created;
        created.root = monster;
        created.controller = controller;
        created.vocalSource = vocalSource;
        created.visual = visual;
        created.body = body;
        created.head = head;
        created.jaw = jaw;
        created.growth = growth;
        created.hitRegions = std::move(hitRegions);
        created.writhingParts = std::move(writhingParts);
        created.writhingBaseRotations = std::move(writhingBaseRotations);
        created.bodyScale = bodyScale;
        created.bodyPosition = Vec3(0.f, bodyY, 0.f);
        created.spawnPosition = spawns[i].position;
        created.headPosition = headPosition;
        created.growthPosition = stalkerGrowthPosition;
        created.archetype = archetype;
        created.idlePhase = randomRange(0.f, 2.f * kPi);
        created.health = 80.f;
        created.wanderTimer = (float)i * 0.9f;
        created.lungeCooldown = 2.5f + (float)i * 1.3f;
        created.wanderTarget = spawns[i].position;
        created.scareArmed = randomRange(0.f, 1.f) < 0.35f;
        _monsters.push_back(created);
    }
}

void DemoLayer::SetState(GameState state) {
    if (_state == state)
        return;

    _state = state;
    if (state == GameState::Play) {
        if (Input::Get())
            Input::Get()->ClearMouseCursorDelta();
        _ignoreMouseDelta = true;
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
        _mouseCaptured = true;
    } else {
        _smoothVelocity = Vec3(0.f);
        _isSprinting = false;
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        _mouseCaptured = false;
    }
    UpdateHUD();
}

void DemoLayer::SetMessage(const std::string& text, float duration) {
    if (_hudMessage) {
        _hudMessage->SetText(String(text));
        _hudMessage->SetTextColor(Color(0.95f, 0.82f, 0.48f, 1.f));
    }
    _messageTimer = duration;
}

void DemoLayer::SetWarning(const std::string& text, float duration) {
    if (_hudWarning) {
        _hudWarning->SetText(String(text));
        _hudWarning->SetTextColor(Color(0.98f, 0.24f, 0.16f, 1.f));
    }
    _warningTimer = duration;
}

bool DemoLayer::HasLineOfSight(const Vec3& from, const Vec3& to, Entity* source) const {
    Vec3 direction = to - from;
    float distance = direction.Length();
    if (distance < 0.001f)
        return true;

    Entity* hit = _scene->GetPhysicalWorld()->RayTestFirstEntity(from, direction.GetNormalized(), distance);
    if (!hit)
        return true;
    if (hit == source || hit == _player)
        return true;

    return false;
}

void DemoLayer::UpdatePlayerCamera(float dt) {
    if (!_player || !_camera || !_flashlight)
        return;

    Input* input = Input::Get();
    const Vec2i mouseDelta = input->GetMouseCursorDelta();
    input->ClearMouseCursorDelta();
    if (_state == GameState::Play && !_ignoreMouseDelta) {
        const int32 maxMouseDelta = 100;
        const int32 deltaX = std::max(-maxMouseDelta, std::min(maxMouseDelta, mouseDelta.x));
        const int32 deltaY = std::max(-maxMouseDelta, std::min(maxMouseDelta, mouseDelta.y));
        _yawDegrees -= (float)deltaX * 0.12f;
        _pitch -= (float)deltaY * 0.12f * kDegToRad;
        _pitch = std::max(-kMaxCameraPitch, std::min(kMaxCameraPitch, _pitch));
    }
    _ignoreMouseDelta = false;

    float yaw = _yawDegrees * kDegToRad;
    float cosPitch = cosf(_pitch);
    Vec3 forward(cosPitch * sinf(yaw), sinf(_pitch), cosPitch * cosf(yaw));
    if (forward.Length() > 0.001f)
        forward.Normalize();
    Vec3 flatForward(sinf(yaw), 0.f, cosf(yaw));
    Vec3 right = Vec3(-flatForward.z, 0.f, flatForward.x);

    Vec3 wish(0.f);
    if (_state == GameState::Play) {
        if (input->IsKeyHold(KEY_CODE_W)) wish.z += 1.f;
        if (input->IsKeyHold(KEY_CODE_S)) wish.z -= 1.f;
        if (input->IsKeyHold(KEY_CODE_D)) wish.x += 1.f;
        if (input->IsKeyHold(KEY_CODE_A)) wish.x -= 1.f;
    }

    const bool movingInput = wish.Length() > 0.001f;
    _isSprinting = _state == GameState::Play && movingInput
        && input->IsKeyHold(KEY_CODE_LSHIFT) && _stamina > 0.f;
    if (_isSprinting)
        _stamina = std::max(0.f, _stamina - 30.f * dt);
    else
        _stamina = std::min(100.f, _stamina + 18.f * dt);

    float speed = _isSprinting ? 6.0f : 4.0f;
    float desiredSpeed = wish.Length() > 0.001f ? speed : 0.f;
    Vec3 desiredVelocity = desiredSpeed > 0.f ? wish.GetNormalized() * speed : Vec3(0.f);
    float response = desiredSpeed > 0.f ? 14.f : 20.f;
    float velocityBlend = 1.f - expf(-response * dt);
    _smoothVelocity += (desiredVelocity - _smoothVelocity) * velocityBlend;
    Vec3 worldVelocity = right * _smoothVelocity.x + flatForward * _smoothVelocity.z;

    if (_state == GameState::Play) {
        auto* controller = _player->GetComponent<CharacterControllerComponent>();
        if (controller)
            controller->Move(Vec2(worldVelocity.x, worldVelocity.z));
    }

    // Footstep sound when moving
    Vec3 playerPos = _player->GetPosition();
    float actualHorizontalSpeed = 0.f;
    if (_hasPreviousPlayerPosition && dt > 0.0001f) {
        Vec3 actualVelocity = (playerPos - _previousPlayerPosition) / dt;
        actualVelocity.y = 0.f;
        actualHorizontalSpeed = actualVelocity.Length();
    }
    _previousPlayerPosition = playerPos;
    _hasPreviousPlayerPosition = true;

    const float stepSpeed = std::max(actualHorizontalSpeed, _smoothVelocity.Length() * 0.75f);
    if (_state == GameState::Play && movingInput && stepSpeed > 0.8f) {
        _footstepTimer -= dt;
        if (_footstepTimer <= 0.f) {
            PlaySfx("footstep");
            _footstepTimer = _isSprinting ? 0.34f : 0.47f;
            _playerNoiseRadius = _isSprinting ? 13.f : 6.5f;
            _playerNoiseTimer = 0.9f;
        }
    } else {
        _footstepTimer = 0.f;
    }
    _bobTime += dt * (_smoothVelocity.Length() > 0.35f ? (_isSprinting ? 12.f : 8.f) : 0.f);

    // Collision sound when hitting a wall
    if (wish.Length() > 0.001f && worldVelocity.Length() < 0.5f) {
        static float collisionCooldown = 0.f;
        collisionCooldown -= dt;
        if (collisionCooldown <= 0.f) {
            PlaySfx("thump");
            collisionCooldown = 0.5f;
            _playerNoiseRadius = 11.f;
            _playerNoiseTimer = 0.8f;
        }
    }

    const bool outsideFloor = fabsf(playerPos.x) > 17.0f || fabsf(playerPos.z) > 12.2f;
    if (playerPos.y < 0.15f || playerPos.y > 20.f || outsideFloor) {
        playerPos = _lastSafePlayerPos;
        _player->SetPosition(playerPos);
        if (auto* controller = _player->GetComponent<CharacterControllerComponent>())
            controller->Teleport(playerPos);
        _smoothVelocity = Vec3(0.f);
        _hasPreviousPlayerPosition = false;
        Logger::Log(LogType::LOG_TYPE_WARN) << "Player fell through, recovered\n";
    } else if (playerPos.y >= 0.3f && playerPos.y < 2.5f
               && fabsf(playerPos.x) <= 16.5f && fabsf(playerPos.z) <= 11.5f) {
        _lastSafePlayerPos = playerPos;
    }

    const float movementBlend = std::min(1.f, _smoothVelocity.Length() / 4.f);
    const float hitShake = _damageFlash * 0.055f;
    const float scareShake = _exitScareTimer > 0.f ? 0.075f : 0.f;
    Vec3 cameraPos = playerPos + Vec3(
        sinf(_bobTime * 0.5f) * 0.012f * movementBlend + sinf(_elapsed * 76.f) * hitShake
            + sinf(_elapsed * 91.f) * scareShake,
        kPlayerEyeHeight + fabsf(sinf(_bobTime)) * 0.025f * movementBlend + cosf(_elapsed * 91.f) * hitShake
            + cosf(_elapsed * 113.f) * scareShake,
        0.f);

    float scareApproach = _exitScareTimer > 0.f ? 1.f - _exitScareTimer / 2.4f : 0.f;
    _camera->SetFOV(90.f + 4.f * _attackPunch + 12.f * scareApproach);

    _cameraEntity->SetPosition(cameraPos);
    _camera->SetPosition(cameraPos);
    Quat cameraRotation = RotationFacing(forward * -1.f);
    _cameraEntity->SetRotation(cameraRotation);
    _camera->SetFront(forward);
    _camera->SetUp(Vec3(0.f, 1.f, 0.f));
    _camera->UpdateMatrices();
    _aimForward = forward;
    Vec3 cameraUp = right.Cross(forward).GetNormalized();

    // Update audio listener
    AudioLayer* audio = AudioLayer::Get();
    if (audio) {
        audio->SetListenerPos(cameraPos);
        audio->SetListenerOrientation(forward, Vec3(0.f, 1.f, 0.f));
    }

    Vec3 headlampPosition = cameraPos + cameraUp * 0.12f - forward * 0.025f;
    _flashlight->SetPosition(headlampPosition);
    _flashlightLight->SetDirection(forward);
    if (_flashlightLens)
        _flashlightLens->SetPosition(headlampPosition);
    if (_firstPersonArm)
    {
        _firstPersonArm->SetPosition(cameraPos + right * (0.23f - 0.08f * _attackPunch)
                                     - cameraUp * (0.40f - 0.025f * _attackPunch)
                                     + forward * (0.35f + 0.42f * _attackPunch));
        _firstPersonArm->SetRotation(RotationFacing(forward));
    }
    if (_firstPersonFist)
    {
        _firstPersonFist->SetPosition(cameraPos + right * (0.20f - 0.10f * _attackPunch)
                                      - cameraUp * (0.34f - 0.06f * _attackPunch)
                                      + forward * (0.50f + 0.48f * _attackPunch));
        _firstPersonFist->SetRotation(RotationFacing(forward));
    }

    _playerVisual->SetRotationEuler(Vec3(0.f, _yawDegrees, 0.f));

    Monster* target = FindAttackTarget(forward);
    if (_hudTarget) {
        _hudTarget->SetVisible(_state == GameState::Play && target != nullptr);
        if (target) {
            const int health = (int)std::ceil(target->health);
            SetHudText(_hudTarget, target->root->GetName() + "  " + std::to_string(health) + "/80 HP",
                       health > 40 ? Color(0.94f, 0.70f, 0.25f, 1.f) : Color(0.96f, 0.22f, 0.16f, 1.f));
        }
    }
}

void DemoLayer::DamagePlayer(float amount) {
    if (_state != GameState::Play || _hurtCooldown > 0.f)
        return;

    _health = std::max(0.f, _health - amount);
    _hurtCooldown = 0.72f;
    _damageFlash = 1.f;
    SetMessage("A SHADOW CLAWS YOU", 0.75f);
    PlaySfx("hurt");

    if (_health <= 0.f) {
        _deaths++;
        SetState(GameState::Lose);
        _messageTimer = 0.f;
        _warningTimer = 0.f;
    }
}

DemoLayer::Monster* DemoLayer::FindAttackTarget(const Vec3& forward) {
    if (!_player)
        return nullptr;

    Vec3 aimForward = forward;
    if (aimForward.Length() < 0.001f)
        return nullptr;
    aimForward.Normalize();

    const Vec3 playerPos = _player->GetPosition();
    const Vec3 eyePosition = playerPos + Vec3(0.f, kPlayerEyeHeight, 0.f);
    float bestDistance = 2.45f;
    Monster* best = nullptr;

    auto IntersectEllipsoid = [&eyePosition, &aimForward](const Vec3& center, const Vec3& scale,
                                                          float yawDegrees, float& distance) {
        const float yaw = yawDegrees * kDegToRad;
        const float cosine = cosf(yaw);
        const float sine = sinf(yaw);
        const Vec3 worldOffset = eyePosition - center;
        const Vec3 localOrigin(worldOffset.x * cosine - worldOffset.z * sine,
                               worldOffset.y,
                               worldOffset.x * sine + worldOffset.z * cosine);
        const Vec3 localDirection(aimForward.x * cosine - aimForward.z * sine,
                                  aimForward.y,
                                  aimForward.x * sine + aimForward.z * cosine);
        const float radiusX = std::max(0.01f, fabsf(scale.x));
        const float radiusY = std::max(0.01f, fabsf(scale.y));
        const float radiusZ = std::max(0.01f, fabsf(scale.z));
        const float originX = localOrigin.x / radiusX;
        const float originY = localOrigin.y / radiusY;
        const float originZ = localOrigin.z / radiusZ;
        const float directionX = localDirection.x / radiusX;
        const float directionY = localDirection.y / radiusY;
        const float directionZ = localDirection.z / radiusZ;
        const float a = directionX * directionX + directionY * directionY + directionZ * directionZ;
        const float b = originX * directionX + originY * directionY + originZ * directionZ;
        const float c = originX * originX + originY * originY + originZ * originZ - 1.f;
        if (c <= 0.f) {
            distance = 0.02f;
            return true;
        }

        const float discriminant = b * b - a * c;
        if (discriminant < 0.f || a < 0.0001f)
            return false;
        const float nearDistance = (-b - sqrtf(discriminant)) / a;
        const float farDistance = (-b + sqrtf(discriminant)) / a;
        distance = nearDistance >= 0.f ? nearDistance : farDistance;
        return distance >= 0.f;
    };

    for (Monster& monster : _monsters) {
        if (!monster.alive || !monster.root)
            continue;

        for (Entity* region : monster.hitRegions) {
            if (!region)
                continue;
            float distance = 0.f;
            if (!IntersectEllipsoid(region->GetAbsolutePosition(), region->GetAbsoluteScale(),
                                    monster.bodyYaw, distance)
                || distance > bestDistance)
                continue;

            const Vec3 targetPoint = eyePosition + aimForward * distance;
            if (!HasLineOfSight(eyePosition, targetPoint, monster.root))
                continue;

            bestDistance = distance;
            best = &monster;
        }
    }

    return best;
}

void DemoLayer::Attack() {
    if (_state != GameState::Play || _attackCooldown > 0.f)
        return;

    _attackCooldown = 0.38f;
    _attackPunch = 1.f;
    _playerNoiseRadius = 9.f;
    _playerNoiseTimer = 0.55f;

    Monster* best = FindAttackTarget(_aimForward);
    if (!best) {
        PlaySfx("swing");
        SetMessage("SWING MISSED - GET CLOSER AND AIM AT THE CREATURE", 0.8f);
        return;
    }

    constexpr float damage = 32.f;
    best->health = std::max(0.f, best->health - damage);
    best->attackAnimation = 1.f;
    _playerNoiseRadius = 12.f;
    _playerNoiseTimer = 0.85f;
    PlaySfx("impact");
    SetMessage("HIT " + best->root->GetName() + "  -" + std::to_string((int)damage)
               + " HP  (" + std::to_string((int)best->health) + "/80)", 1.0f);
    if (best->health <= 0.f) {
        best->alive = false;
        if (best->controller)
            best->controller->Deactivate();
        best->root->SetActive(false);
        SetMessage(best->root->GetName() + " - KILLED", 1.4f);
    }
}

void DemoLayer::UpdateMonsters(float dt) {
    if (_state != GameState::Play || !_player)
        return;

    Vec3 playerPos = _player->GetPosition();
    Vec3 playerEye = playerPos + Vec3(0.f, 0.72f, 0.f);
    const float yaw = _yawDegrees * kDegToRad;
    const Vec3 forward(cosf(_pitch) * sinf(yaw), sinf(_pitch), cosf(_pitch) * cosf(yaw));
    bool anyHunting = false;
    bool scareTriggered = false;
    bool anyHeard = false;
    bool anySawLight = false;
    bool anySawPlayer = false;

    for (Monster& monster : _monsters) {
        if (!monster.alive || !monster.root)
            continue;

        monster.attackCooldown = std::max(0.f, monster.attackCooldown - dt);
        monster.attackAnimation = std::max(0.f, monster.attackAnimation - dt * 3.2f);
        monster.lungeCooldown = std::max(0.f, monster.lungeCooldown - dt);
        monster.lungeTime = std::max(0.f, monster.lungeTime - dt);
        monster.alertTime = std::max(0.f, monster.alertTime - dt);
        monster.lightNoticeCooldown = std::max(0.f, monster.lightNoticeCooldown - dt);
        Vec3 monsterPos = monster.root->GetPosition();

        const uint32 monsterIndex = static_cast<uint32>(&monster - _monsters.data());

        const bool outsideFloor = fabsf(monsterPos.x) > 17.0f || fabsf(monsterPos.z) > 12.2f;
        if (monsterPos.y < 0.25f || monsterPos.y > 20.f || outsideFloor) {
            const uint32 idx = monsterIndex;
            monster.root->SetPosition(_lastSafeMonsterPos[idx]);
            if (monster.controller)
                monster.controller->Teleport(_lastSafeMonsterPos[idx]);
            monsterPos = _lastSafeMonsterPos[idx];
            Logger::Log(LogType::LOG_TYPE_WARN) << "Monster " << idx << " fell through, recovered\n";
        } else if (monsterPos.y >= 0.3f && monsterPos.y < 2.5f
                   && fabsf(monsterPos.x) <= 16.5f && fabsf(monsterPos.z) <= 11.5f) {
            _lastSafeMonsterPos[monsterIndex] = monsterPos;
        }
        Vec3 toPlayer = playerPos - monsterPos;
        float distance = toPlayer.Length();
        Vec3 eye = monsterPos + Vec3(0.f, 0.78f, 0.f);
        Vec3 flatToPlayer(toPlayer.x, 0.f, toPlayer.z);
        const float planarDistance = flatToPlayer.Length();
        if (planarDistance > 0.001f)
            flatToPlayer /= planarDistance;
        const Vec3 monsterForward(sinf(monster.bodyYaw * kDegToRad), 0.f,
                                  cosf(monster.bodyYaw * kDegToRad));
        const float facingDot = flatToPlayer.Dot(monsterForward);
        const bool visible = distance < 11.f && HasLineOfSight(eye, playerEye, monster.root)
            && (distance < 2.8f || facingDot > 0.42f);
        Vec3 toMonsterEye = eye - playerEye;
        const float lightDot = toMonsterEye.Length() > 0.001f
            ? forward.Dot(toMonsterEye.GetNormalized()) : 1.f;
        const bool illuminated = _flashlightEnabled && distance < 18.f && lightDot > 0.88f
            && HasLineOfSight(playerEye, eye, monster.root);
        const bool heard = _playerNoiseTimer > 0.f && distance < _playerNoiseRadius;
        anyHeard = anyHeard || heard;
        anySawLight = anySawLight || illuminated;
        anySawPlayer = anySawPlayer || visible;
        if (visible || illuminated || heard) {
            monster.alertTime = std::max(monster.alertTime, illuminated ? 5.f : 3.5f);
            monster.lastKnownPlayerPosition = playerPos;
        }
        const bool alerted = monster.alertTime > 0.f;
        if (alerted && !monster.vocalAlerted)
            PlayMonsterVocal(monster, false);
        monster.vocalAlerted = alerted;
        const bool spottedByLight = illuminated && !visible;
        if (spottedByLight && monster.lightNoticeCooldown <= 0.f) {
            monster.lightNoticeCooldown = 4.f;
            PlaySfx("scare");
            if (_warningTimer <= 0.f)
                SetWarning("THE BEAM DRAWS THEM", 1.2f);
        }
        if ((visible || illuminated) && distance < 6.f && distance > 1.45f && monster.lungeCooldown <= 0.f) {
            const float lungeDuration[3] = { 0.55f, 0.82f, 0.68f };
            const float lungeCooldown[3] = { 4.2f, 6.0f, 3.6f };
            monster.lungeTime = lungeDuration[monster.archetype];
            monster.lungeCooldown = lungeCooldown[monster.archetype];
            PlayMonsterVocal(monster, true);
            const char* warning[3] = { "THE STALKER IS CLOSING", "THE BRUTE IS CHARGING", "SOMETHING IS CRAWLING AT YOU" };
            SetWarning(warning[monster.archetype], 0.9f);
        }
        if ((visible || illuminated) && distance > 2.2f && distance < 7.5f
            && monster.scareArmed && !monster.scarePlayed && _scareCooldown <= 0.f) {
            monster.scarePlayed = true;
            _scareCooldown = 12.f;
            scareTriggered = true;
            PlaySfx("scare");
            SetWarning("IT FOUND YOU", 1.1f);
        }
        const float phase = _elapsed * (alerted ? 8.5f : 2.4f) + monster.idlePhase;
        if (monster.body) {
            const float breathAmount[3] = { 0.038f, 0.022f, 0.052f };
            const float breath = 1.f + sinf(phase * (monster.archetype == 1 ? 0.62f : 0.86f)) * breathAmount[monster.archetype];
            monster.body->SetScale(Vec3(monster.bodyScale.x * breath,
                                        monster.bodyScale.y / breath,
                                        monster.bodyScale.z * breath));
            const float bodyBob = sinf(phase * 0.48f) * (monster.archetype == 1 ? 0.008f : 0.025f);
            monster.body->SetPosition(monster.bodyPosition + Vec3(
                monster.archetype == 2 ? sinf(phase * 0.31f) * 0.025f : 0.f,
                bodyBob, 0.f));
        }
        for (size_t partIndex = 0; partIndex < monster.writhingParts.size(); partIndex++) {
            Entity* part = monster.writhingParts[partIndex];
            if (!part || partIndex >= monster.writhingBaseRotations.size())
                continue;
            const Vec3& baseRotation = monster.writhingBaseRotations[partIndex];
            const float partPhase = phase + static_cast<float>(partIndex) * 1.7f + monsterIndex;
            const float motion = monster.archetype == 0 ? 1.25f : (monster.archetype == 1 ? 0.62f : 1.65f);
            part->SetRotationEuler(Vec3(baseRotation.x + sinf(partPhase * 0.73f) * 14.f * motion,
                                        baseRotation.y + sinf(partPhase) * 24.f * motion,
                                        baseRotation.z + cosf(partPhase * 0.81f) * 18.f * motion));
        }

        Vec3 moveDirection(0.f);
        float speed = 0.f;
        if (alerted) {
            anyHunting = true;
            const Vec3 targetPosition = (visible || illuminated || heard)
                ? playerPos : monster.lastKnownPlayerPosition;
            moveDirection = Vec3(targetPosition.x - monsterPos.x, 0.f, targetPosition.z - monsterPos.z);
            if (moveDirection.Length() > 0.001f)
                moveDirection.Normalize();
            const float targetDistance = (targetPosition - monsterPos).Length();
            const float huntSpeed[3] = { 3.8f, 2.55f, 3.3f };
            const float lungeSpeed[3] = { 5.0f, 4.6f, 5.9f };
            speed = targetDistance > 1.2f
                ? (monster.lungeTime > 0.f ? lungeSpeed[monster.archetype]
                   : (heard && !visible ? huntSpeed[monster.archetype] * 0.72f : huntSpeed[monster.archetype]))
                : 0.f;
            monster.wanderTimer = 0.f;
        } else {
            monster.wanderTimer -= dt;
            if (monster.wanderTimer <= 0.f) {
                float angle = Hash2D((uint32)(_elapsed * 1000.f), (uint32)(monsterPos.x * 13.f + 100.f), 711u) * 2.f * kPi;
                float radius = 3.0f + Hash2D((uint32)(monsterPos.z * 17.f + 100.f), (uint32)(_elapsed * 1000.f), 913u) * 5.0f;
                monster.wanderTarget = monsterPos + Vec3(cosf(angle) * radius, 0.f, sinf(angle) * radius);
                monster.wanderTarget.x = std::max(-15.7f, std::min(15.7f, monster.wanderTarget.x));
                monster.wanderTarget.z = std::max(-10.7f, std::min(10.7f, monster.wanderTarget.z));
                monster.wanderTimer = 3.2f + Hash2D((uint32)(_elapsed * 10.f), (uint32)(monsterPos.z * 31.f), 107u) * 3.5f;
            }

            moveDirection = Vec3(monster.wanderTarget.x - monsterPos.x, 0.f, monster.wanderTarget.z - monsterPos.z);
            if (moveDirection.Length() > 0.35f) {
                moveDirection.Normalize();
                const float wanderSpeed[3] = { 1.5f, 0.72f, 1.22f };
                speed = wanderSpeed[monster.archetype];
            }
        }

        Vec3 targetVelocity = moveDirection * speed;
        const float response = speed > monster.movementVelocity.Length() ? 3.0f : 5.5f;
        const float velocityBlend = 1.f - expf(-response * dt);
        monster.movementVelocity += (targetVelocity - monster.movementVelocity) * velocityBlend;
        if (monster.controller)
            monster.controller->Move(Vec2(monster.movementVelocity.x, monster.movementVelocity.z));

        if (planarDistance > 0.001f) {
            Vec3 bodyDirection = monster.movementVelocity;
            bodyDirection.y = 0.f;
            if (bodyDirection.Length() < 0.12f && alerted)
                bodyDirection = flatToPlayer;
            if (bodyDirection.Length() < 0.12f)
                bodyDirection = monsterForward;
            bodyDirection.Normalize();

            const float targetBodyYaw = atan2f(bodyDirection.x, bodyDirection.z) * kRadToDeg;
            if (!monster.bodyYawInitialized) {
                monster.bodyYaw = targetBodyYaw;
                monster.bodyYawInitialized = true;
            } else {
                const float turnRate = alerted ? 240.f : 165.f;
                const float turn = AngleDeltaDegrees(targetBodyYaw, monster.bodyYaw);
                monster.bodyYaw += std::max(-turnRate * dt, std::min(turnRate * dt, turn));
            }
            if (monster.visual) {
                const float attackLean = monster.archetype == 1 ? -17.f : (monster.archetype == 0 ? 11.f : 19.f);
                const float idleSway = sinf(phase * 0.42f) * (monster.archetype == 1 ? 1.5f : 4.f);
                monster.visual->SetRotationEuler(Vec3(monster.attackAnimation * attackLean + idleSway,
                                                       monster.bodyYaw,
                                                       monster.attackAnimation * (monster.archetype == 1 ? 7.f : 18.f)
                                                           + sinf(phase * 0.31f) * (monster.archetype == 2 ? 8.f : 2.5f)));
            }

            const Vec3 lookTarget = (visible || illuminated || heard)
                ? playerEye : monster.lastKnownPlayerPosition + Vec3(0.f, 0.72f, 0.f);
            if (alerted) {
                const Vec3 toLookTarget(lookTarget.x - monsterPos.x, 0.f, lookTarget.z - monsterPos.z);
                const float lookYaw = atan2f(toLookTarget.x, toLookTarget.z) * kRadToDeg;
                const float desiredHeadYaw = std::max(-78.f, std::min(78.f, AngleDeltaDegrees(lookYaw, monster.bodyYaw)));
                monster.headYaw += (desiredHeadYaw - monster.headYaw) * (1.f - expf(-10.f * dt));
            } else {
                const float scanRange[3] = { 38.f, 18.f, 55.f };
                const float scanSpeed[3] = { 0.53f, 0.24f, 0.82f };
                const float idleYaw = sinf(_elapsed * scanSpeed[monster.archetype] + monster.idlePhase) * scanRange[monster.archetype];
                monster.headYaw += (idleYaw - monster.headYaw) * (1.f - expf(-2.2f * dt));
            }
            if (monster.head) {
                Vec3 toPlayerEye = lookTarget - (monsterPos + monster.headPosition);
                const float lookPitch = atan2f(toPlayerEye.y, std::max(0.001f, planarDistance)) * kRadToDeg;
                const float idlePitch[3] = { 4.f, -2.f, 8.f };
                const float twitch = sinf(_elapsed * (alerted ? 3.7f : 1.8f) + monster.idlePhase * 2.f) * (alerted ? 2.5f : 4.5f);
                const float pitch = alerted ? -lookPitch : idlePitch[monster.archetype] + sinf(phase * 0.35f) * 5.f;
                monster.head->SetRotationEuler(Vec3(std::max(-30.f, std::min(30.f, pitch + twitch + monster.attackAnimation * -12.f)),
                                                    monster.headYaw + twitch,
                                                    sinf(phase * 0.7f) * (alerted ? 3.f : 7.f)));
            }
            if (monster.jaw) {
                const float bite = monster.attackAnimation * (monster.archetype == 1 ? 18.f : 30.f);
                const float idleSnarl = 2.5f + (0.5f + 0.5f * sinf(phase * 0.65f)) * 5.f;
                monster.jaw->SetRotationEuler(Vec3(idleSnarl + bite, 0.f,
                                                   sinf(phase * 0.42f) * 2.5f));
            }

            const float stride = std::min(1.f, monster.movementVelocity.Length() / 2.3f);
            if (monster.growth) {
                monster.growth->SetPosition(monster.growthPosition
                    + Vec3(0.f, sinf(phase * 0.8f) * (0.012f + 0.045f * stride), 0.f));
                monster.growth->SetRotationEuler(Vec3(sinf(phase) * (3.f + 6.f * stride), 0.f,
                                                       cosf(phase * 0.5f) * (4.f + 7.f * stride)));
            }
        }

        const float attackRange[3] = { 1.30f, 1.70f, 1.48f };
        if (alerted && distance < attackRange[monster.archetype] && monster.attackCooldown <= 0.f) {
            const float attackCooldown[3] = { 0.95f, 1.75f, 1.08f };
            const float attackDamage[3] = { 12.f, 24.f, 16.f };
            monster.attackCooldown = attackCooldown[monster.archetype];
            monster.attackAnimation = 1.f;
            DamagePlayer(attackDamage[monster.archetype]);
        }
    }

    if (anyHunting && !scareTriggered && _warningTimer <= 0.f) {
        const char* warning = anySawLight ? "THE LIGHT DRAWS THEM"
            : (anySawPlayer ? "IT CAN SEE YOU" : (anyHeard ? "THEY HEARD YOU" : "IT IS CLOSING IN"));
        SetWarning(warning, 0.9f);
    }
}

void DemoLayer::UpdateArtifacts(float dt) {
    _elapsed += dt;

    for (uint32 i = 0; i < _artifacts.size(); i++) {
        Artifact& artifact = _artifacts[i];
        if (artifact.collected || !artifact.root)
            continue;

        Vec3 bob = artifact.basePosition + Vec3(0.f, sinf(_elapsed * 2.15f + (float)i * 1.7f) * 0.11f, 0.f);
        artifact.root->SetPosition(bob);
        artifact.root->SetRotationEuler(Vec3(0.f, _elapsed * 42.f + (float)i * 61.f, _elapsed * 23.f));

        if (_state == GameState::Play) {
            Vec3 delta = bob - _player->GetPosition();
            if (delta.Length() < 1.55f) {
                artifact.collected = true;
                artifact.root->SetActive(false);
                _artifactsCollected++;
                if (_ambientSource)
                    _ambientSource->SetPitch(1.f + 0.12f * _artifactsCollected);
                PlaySfx("pickup");
                SetMessage("RECOVERED: " + artifact.name + "  (" + std::to_string(_artifactsCollected) + "/5)", 1.8f);
                if (_artifactsCollected >= 5)
                    OpenDoor();
            }
        }
    }

    for (Pickup& pickup : _pickups) {
        if (pickup.collected || !pickup.root)
            continue;

        const float phase = _elapsed * 2.6f + pickup.phase;
        const Vec3 bob = pickup.basePosition + Vec3(0.f, sinf(phase) * 0.09f, 0.f);
        pickup.root->SetPosition(bob);
        pickup.root->SetRotationEuler(Vec3(0.f, _elapsed * 52.f + pickup.phase * kRadToDeg, 0.f));

        if (_state != GameState::Play || (bob - _player->GetPosition()).Length() > 1.15f)
            continue;

        const bool canCollect = pickup.type == PickupType::Health ? _health < 100.f : _stamina < 100.f;
        if (!canCollect)
            continue;

        pickup.collected = true;
        pickup.root->SetActive(false);
        if (pickup.type == PickupType::Health) {
            const float restored = std::min(35.f, 100.f - _health);
            _health += restored;
            PlaySfx("health_pickup");
            SetMessage("BLOOD TONIC  +" + std::to_string((int)restored) + " HEALTH", 1.6f);
        } else {
            const float restored = std::min(45.f, 100.f - _stamina);
            _stamina += restored;
            PlaySfx("stamina_pickup");
            SetMessage("GREEN SALTS  +" + std::to_string((int)restored) + " STAMINA", 1.6f);
        }
    }
}

void DemoLayer::OpenDoor() {
    if (_doorOpen)
        return;

    _doorOpen = true;
    if (_exitDoor)
        _exitDoor->SetPosition(Vec3(0.f, -2.85f, -11.72f));

    PlaySfx("door_open");
    SetWarning("THE NORTH DOOR GRINDS OPEN", 2.4f);
    SetMessage("Return to the north exit", 3.5f);

    IRenderer* renderer = Application::Get()->GetRenderer();
    if (renderer)
        renderer->QueueSkyProbeUpdate();
}

void DemoLayer::CheckExit() {
    if (_state != GameState::Play || !_player)
        return;

    Vec3 playerPos = _player->GetPosition();

    if (_doorOpen && playerPos.z < -10.65f) {
        _exitScareTimer = 2.4f;
        if (_exitScreamer) {
            Vec3 forward = _aimForward.GetNormalized();
            _exitScreamer->SetPosition(_camera->GetPosition() + forward * 1.65f);
            _exitScreamer->SetRotation(RotationFacing(forward * -1.f));
            _exitScreamer->SetActive(true);
        }
        _damageFlash = 1.f;
        PlaySfx("exit_scare");
        SetMessage("IT FOLLOWED YOU OUT", 3.0f);
        SetWarning("DON'T TURN AROUND", 2.8f);
        SetState(GameState::Win);
        if (_ambientSource && _endingMusicClip) {
            _ambientSource->Stop();
            _ambientSource->SetAudioClip(_endingMusicClip);
            _ambientSource->SetPitch(1.f);
            _ambientSource->SetVolume(0.72f);
            _ambientSource->SetLoop(true);
            _ambientSource->Play();
            if (_ambientSource->IsPlaying())
                Logger::Log(LogType::LOG_TYPE_INFO) << "House of Shadows: switched to ending music\n";
            else
                Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: ending music did not start\n";
        }
        return;
    }

    if (!_doorOpen && playerPos.z < -10.15f && _lockedDoorMessageCooldown <= 0.f) {
        _lockedDoorMessageCooldown = 4.0f;
        SetMessage("SEALED - RECOVER ALL FIVE ARTIFACTS", 2.0f);
    }
}

void DemoLayer::ResetGame() {
    _health = 100.f;
    _stamina = 100.f;
    _attackCooldown = 0.f;
    _hurtCooldown = 0.f;
    _damageFlash = 0.f;
    _exitScareTimer = 0.f;
    if (_exitScreamer) {
        _exitScreamer->SetActive(false);
        _exitScreamer->SetPosition(Vec3(0.f));
        _exitScreamer->SetScale(Vec3(1.f));
    }
    _attackPunch = 0.f;
    _bobTime = 0.f;
    _elapsed = 0.f;
    _messageTimer = 0.f;
    _warningTimer = 0.f;
    _lockedDoorMessageCooldown = 0.f;
    _footstepTimer = 0.f;
    _alternateFootstep = false;
    _isSprinting = false;
    _scareCooldown = 6.f;
    _playerNoiseRadius = 0.f;
    _playerNoiseTimer = 0.f;
    _hasPreviousPlayerPosition = false;
    _artifactsCollected = 0;
    _doorOpen = false;
    if (_ambientSource && _ambientMusicClip) {
        _ambientSource->Stop();
        _ambientSource->SetAudioClip(_ambientMusicClip);
        _ambientSource->SetPitch(1.f);
        _ambientSource->SetVolume(0.58f);
        _ambientSource->SetLoop(true);
        _ambientSource->Play();
    }
    _smoothVelocity = Vec3(0.f);
    _yawDegrees = 180.f;
    _pitch = 0.f;
    _aimForward = Vec3(0.f, 0.f, -1.f);
    _lastSafePlayerPos = Vec3(0.f, kPlayerGroundHeight, 7.0f);
    Input* input = Input::Get();
    if (input)
        input->ClearMouseCursorDelta();

    if (_exitDoor)
        _exitDoor->SetPosition(Vec3(0.f, 1.62f, -11.72f));

    for (Artifact& artifact : _artifacts) {
        artifact.collected = false;
        if (artifact.root) {
            artifact.root->SetActive(true);
            artifact.root->SetPosition(artifact.basePosition);
        }
    }
    for (Pickup& pickup : _pickups) {
        pickup.collected = false;
        if (pickup.root) {
            pickup.root->SetActive(true);
            pickup.root->SetPosition(pickup.basePosition);
            pickup.root->SetRotationEuler(Vec3(0.f));
        }
    }

    for (uint32 i = 0; i < _monsters.size(); i++) {
        Monster& monster = _monsters[i];
        monster.health = 80.f;
        monster.attackCooldown = 0.f;
        monster.attackAnimation = 0.f;
        monster.lungeCooldown = 2.5f + (float)i * 1.3f;
        monster.lungeTime = 0.f;
        monster.wanderTimer = (float)i * 0.8f;
        monster.wanderTarget = monster.spawnPosition;
        monster.movementVelocity = Vec3(0.f);
        monster.bodyYawInitialized = false;
        monster.headYaw = 0.f;
        monster.scarePlayed = false;
        monster.vocalAlerted = false;
        monster.alive = true;
        _lastSafeMonsterPos[i] = monster.spawnPosition;
        if (monster.root)
            monster.root->SetActive(true);
        if (monster.controller) {
            monster.controller->Teleport(monster.spawnPosition);
            monster.controller->Activate();
        }
    }

    if (_player) {
        _player->SetRotationEuler(Vec3(0.f));
        if (auto* controller = _player->GetComponent<CharacterControllerComponent>())
            controller->Teleport(Vec3(0.f, kPlayerGroundHeight, 7.0f));
    }
    if (_playerVisual) {
        _playerVisual->SetRotationEuler(Vec3(0.f, _yawDegrees, 0.f));
    }
    if (_cameraEntity) {
        _cameraEntity->SetPosition(Vec3(0.f, kPlayerGroundHeight + kPlayerEyeHeight, 7.0f));
        _cameraEntity->SetRotation(RotationFacing(Vec3(0.f, 0.f, 1.f)));
    }
    if (_camera) {
        _camera->SetPosition(Vec3(0.f, kPlayerGroundHeight + kPlayerEyeHeight, 7.0f));
        _camera->SetFront(Vec3(0.f, 0.f, -1.f));
        _camera->SetUp(Vec3(0.f, 1.f, 0.f));
        _camera->SetFOV(90.f);
        _camera->UpdateMatrices();
    }

    SetMessage("The house remembers your footsteps", 2.2f);
    SetState(GameState::Play);
    if (Input::Get())
        Input::Get()->ClearMouseCursorDelta();
    _ignoreMouseDelta = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);
    _mouseCaptured = true;
}

void DemoLayer::StartGame() {
    if (_state != GameState::Menu)
        return;

    SetState(GameState::Play);
    _messageTimer = 0.f;
    _warningTimer = 0.f;
    _footstepTimer = 0.f;
    _hasPreviousPlayerPosition = false;
    SetMessage("Find the five cursed artifacts", 2.6f);
    UpdateHUD();
    if (Input::Get())
        Input::Get()->ClearMouseCursorDelta();
    _ignoreMouseDelta = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);
    _mouseCaptured = true;
}

void DemoLayer::TogglePause() {
    if (_state == GameState::Play) {
        SetState(GameState::Pause);
        SetWarning("PAUSED", 999.f);
    } else if (_state == GameState::Pause) {
        SetState(GameState::Play);
        _warningTimer = 0.f;
    }
    UpdateHUD();
}

void DemoLayer::PlaySfx(const std::string& clipName) {
    AudioSourceComponent* source = clipName == "footstep" ? _footstepSource : _sfxSource;
    if (!source)
        return;

    const char* category = nullptr;
    if (clipName == "footstep")
        category = "steps";
    else if (clipName == "swing" || clipName == "impact" || clipName == "thump")
        category = "attack";
    else if (clipName == "scare" || clipName == "exit_scare")
        category = "scare";
    else if (clipName == "pickup" || clipName == "health_pickup" || clipName == "stamina_pickup")
        category = "pickups";
    else if (clipName == "door_open")
        category = "doors";
    else if (clipName == "flashlight")
        category = "ui";

    AudioClipResource* clip = category ? GetCustomAudioClip(category) : nullptr;
    if (!clip)
        clip = ResourceCache::Get()->GetResource<AudioClipResource>(clipName);
    if (!clip) {
        Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: missing audio clip '" << clipName << "'\n";
        return;
    }

    if (clipName == "footstep") {
        source->SetPitch(_alternateFootstep ? 1.04f : 0.94f);
        _alternateFootstep = !_alternateFootstep;
    } else if (clipName == "pickup") {
        source->SetPitch(0.88f + 0.06f * (float)_artifactsCollected);
    } else if (clipName == "health_pickup") {
        source->SetPitch(0.96f);
    } else if (clipName == "stamina_pickup") {
        source->SetPitch(1.08f);
    } else {
        source->SetPitch(1.f);
    }
    if (source->IsPlaying())
        source->Stop();
    source->SetAudioClip(clip);
    source->Play();
}

void DemoLayer::PlayMonsterVocal(Monster& monster, bool attack) {
    if (!monster.vocalSource)
        return;

    const char* clips[3] = { "stalker_vocal", "brute_vocal", "crawler_vocal" };
    const float basePitch[3] = { 1.12f, 0.72f, 1.42f };
    const int archetype = std::max(0, std::min(2, monster.archetype));
    if (monster.vocalSource->IsPlaying())
        monster.vocalSource->Stop();
    monster.vocalSource->SetPitch(basePitch[archetype] * (attack ? 0.88f : 1.f));
    AudioClipResource* clip = GetCustomAudioClip(attack ? "monsters_attack" : "monsters");
    if (!clip)
        clip = ResourceCache::Get()->GetResource<AudioClipResource>(clips[archetype]);
    if (!clip) {
        Logger::Log(LogType::LOG_TYPE_WARN) << "House of Shadows: missing monster vocal '" << clips[archetype] << "'\n";
        return;
    }
    monster.vocalSource->SetAudioClip(clip);
    monster.vocalSource->Play();
}

AudioClipResource* DemoLayer::GetCustomAudioClip(const std::string& category) const {
    const auto found = _customAudioClips.find(category);
    if (found == _customAudioClips.end() || found->second.empty())
        return nullptr;

    static std::mt19937 randomEngine{ std::random_device{}() };
    std::uniform_int_distribution<size_t> chooseClip(0, found->second.size() - 1);
    return found->second[chooseClip(randomEngine)];
}

void DemoLayer::UpdateTorchFlicker(float dt) {
    _flickerTime += dt;
    for (RoomMotion& motion : _roomMotion) {
        if (!motion.entity)
            continue;
        const float phase = _elapsed * motion.speed + motion.phase;
        motion.entity->SetRotationEuler(motion.baseRotation
            + Vec3(sinf(phase * 0.73f) * motion.sway,
                   sinf(phase * 0.41f) * motion.sway * 0.45f,
                   cosf(phase) * motion.sway));
        if (motion.pulse > 0.f) {
            const float beat = 1.f + sinf(phase * 2.1f) * motion.pulse;
            motion.entity->SetScale(Vec3(motion.baseScale.x * beat,
                                         motion.baseScale.y / beat,
                                         motion.baseScale.z * beat));
        }
    }

    for (Torch& torch : _torches) {
        float wave = sinf(_flickerTime * 7.3f + torch.phase) * 0.5f
            + sinf(_flickerTime * 13.1f + torch.phase * 2.7f) * 0.32f
            + sinf(_flickerTime * 23.7f + torch.phase * 4.1f) * 0.18f;
        float intensity = torch.baseIntensity * (1.0f + wave * 0.22f);
        if (torch.light)
            torch.light->GetIntensity() = std::max(0.55f, intensity);
        if (torch.root)
            torch.root->SetScale(Vec3(0.15f + 0.025f * wave, 0.22f + 0.035f * wave, 0.15f + 0.025f * wave));
    }
}

TextView* MakeHudView(View* parent, const Vec2& position, const Vec2& size, const Color& color, const std::string& text) {
    auto* view = new TextView();
    view->SetBounds(Rect(position, size));
    view->SetFontName("arial");
    view->SetTextColor(color);
    view->SetText(String(text));
    parent->AddChild(view);
    return view;
}

void DemoLayer::CreateHUD() {
    auto* ui = UiLayer::Get();
    if (!ui) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: UI layer unavailable\n";
        return;
    }

    View* root = ui->GetRootView();
    const Color parchment(0.92f, 0.78f, 0.48f, 1.f);
    const Color blood(0.96f, 0.18f, 0.12f, 1.f);
    const Color bone(0.82f, 0.84f, 0.76f, 1.f);

    auto* bloodOverlay = new SpriteView();
    TextureResource* bloodTexture = ResourceCache::Get()->GetResource<TextureResource>("EndingBloodOverlay");
    if (bloodTexture) {
        bloodOverlay->SetSprite(bloodTexture->getId().toString());
    } else {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "House of Shadows: ending blood overlay texture is unavailable\n";
    }
    bloodOverlay->SetBounds(Rect(Vec2(0.f, 0.f), Vec2((float)kScreenWidth, (float)kScreenHeight)));
    bloodOverlay->SetVisible(false);
    root->AddChild(bloodOverlay);
    _hudBloodOverlay = bloodOverlay;

    _hudTitle = MakeHudView(root, Vec2(330.f, 92.f), Vec2(940.f, 112.f), parchment, "HOUSE OF SHADOWS");
    _hudSubtitle = MakeHudView(root, Vec2(330.f, 205.f), Vec2(940.f, 52.f), bone, "A NIGHT WITHOUT MERCY");
    _hudPrompt = MakeHudView(root, Vec2(430.f, 690.f), Vec2(740.f, 72.f), parchment, "CLICK OR PRESS ENTER TO ENTER");
    _hudControls = MakeHudView(root, Vec2(28.f, 835.f), Vec2(1520.f, 32.f), bone,
                               "WASD MOVE   MOUSE AIM   SHIFT SPRINT   SPACE/LMB PUNCH   F FLASHLIGHT   ESC PAUSE   R RESTART");
    _hudObjective = MakeHudView(root, Vec2(28.f, 24.f), Vec2(740.f, 38.f), parchment, "FIND THE FIVE ARTIFACTS");
    _hudArtifacts = MakeHudView(root, Vec2(28.f, 70.f), Vec2(430.f, 32.f), bone, "ARTIFACTS 0/5");
    const char* inventoryNames[5] = {
        "BROKEN LOCKET", "BONE KEY", "GLASS EYE", "ASHEN IDOL", "CURSED BELL"
    };
    for (int i = 0; i < 5; i++)
        _hudInventory[i] = MakeHudView(root, Vec2(28.f, 112.f + i * 27.f), Vec2(440.f, 26.f),
                                       bone, std::string("[ ]  ") + inventoryNames[i]);
    _hudHealth = MakeHudView(root, Vec2(1115.f, 24.f), Vec2(455.f, 38.f), Color(0.84f, 0.28f, 0.20f, 1.f), "HEALTH 100");
    _hudStamina = MakeHudView(root, Vec2(1115.f, 70.f), Vec2(455.f, 32.f), Color(0.92f, 0.76f, 0.34f, 1.f), "STAMINA 100");
    _hudFlashlight = MakeHudView(root, Vec2(1115.f, 112.f), Vec2(455.f, 30.f),
                                 Color(0.92f, 0.76f, 0.34f, 1.f), "FLASHLIGHT ON [F]");
    _hudWarning = MakeHudView(root, Vec2(180.f, 128.f), Vec2(1240.f, 44.f), blood, "THEY HAVE YOUR SCENT");
    _hudMessage = MakeHudView(root, Vec2(190.f, 640.f), Vec2(1220.f, 44.f), parchment, "");
    _hudScareText = MakeHudView(root, Vec2(220.f, 350.f), Vec2(1160.f, 110.f), blood, "");
    _hudScareEcho = MakeHudView(root, Vec2(300.f, 462.f), Vec2(1000.f, 56.f), bone, "");
    _hudCrosshair = MakeHudView(root, Vec2(788.f, 438.f), Vec2(24.f, 24.f), Color(0.94f, 0.70f, 0.25f, 1.f), "+");
    _hudTarget = MakeHudView(root, Vec2(540.f, 488.f), Vec2(520.f, 34.f), parchment, "");

    // Hide all HUD elements initially, UpdateHUD will show the right ones
    if (_hudTitle) _hudTitle->SetVisible(false);
    if (_hudSubtitle) _hudSubtitle->SetVisible(false);
    if (_hudPrompt) _hudPrompt->SetVisible(false);
    if (_hudControls) _hudControls->SetVisible(false);
    if (_hudObjective) _hudObjective->SetVisible(false);
    if (_hudArtifacts) _hudArtifacts->SetVisible(false);
    for (TextView* item : _hudInventory)
        if (item) item->SetVisible(false);
    if (_hudHealth) _hudHealth->SetVisible(false);
    if (_hudStamina) _hudStamina->SetVisible(false);
    if (_hudFlashlight) _hudFlashlight->SetVisible(false);
    if (_hudWarning) _hudWarning->SetVisible(false);
    if (_hudMessage) _hudMessage->SetVisible(false);
    if (_hudScareText) _hudScareText->SetVisible(false);
    if (_hudScareEcho) _hudScareEcho->SetVisible(false);
    if (_hudCrosshair) _hudCrosshair->SetVisible(false);
    if (_hudTarget) _hudTarget->SetVisible(false);
}

void DemoLayer::SetHudText(TextView* view, const std::string& text, const Color& color) {
    if (!view)
        return;
    view->SetText(String(text));
    view->SetTextColor(color);
}

void DemoLayer::UpdateHUD() {
    bool inMenu = _state == GameState::Menu;
    bool inPlay = _state == GameState::Play;
    bool inPause = _state == GameState::Pause;
    bool finished = _state == GameState::Win || _state == GameState::Lose;
    bool gameplayHud = inPlay || inPause;

    if (_hudTitle) {
        _hudTitle->SetVisible(inMenu || (finished && _exitScareTimer <= 0.f));
        SetHudText(_hudTitle, _state == GameState::Win ? "YOU GOT OUT" : (_state == GameState::Lose ? "THE HOUSE KEEPS YOU" : "HOUSE OF SHADOWS"),
                   _state == GameState::Lose ? Color(0.88f, 0.14f, 0.08f, 1.f) : Color(0.92f, 0.78f, 0.48f, 1.f));
    }
    if (_hudSubtitle) {
        _hudSubtitle->SetVisible(inMenu || (finished && _exitScareTimer <= 0.f));
        SetHudText(_hudSubtitle,
                   _state == GameState::Win ? "IT CAME THROUGH WITH YOU" : (_state == GameState::Lose ? "NO ONE WILL FIND YOU" : "A NIGHT WITHOUT MERCY"),
                   Color(0.82f, 0.84f, 0.76f, 1.f));
    }
    if (_hudPrompt) {
        _hudPrompt->SetVisible(inMenu || (finished && _exitScareTimer <= 0.f));
        SetHudText(_hudPrompt, finished ? "CLICK OR PRESS R TO TRY AGAIN" : "CLICK OR PRESS ENTER TO ENTER", Color(0.92f, 0.78f, 0.48f, 1.f));
    }

    if (_hudObjective) {
        _hudObjective->SetVisible(gameplayHud);
        SetHudText(_hudObjective,
                   _doorOpen ? "RETURN TO THE NORTH EXIT" : "RECOVER THE FIVE ARTIFACTS",
                   _doorOpen ? Color(0.48f, 0.98f, 0.58f, 1.f) : Color(0.92f, 0.78f, 0.48f, 1.f));
    }
    if (_hudArtifacts) {
        _hudArtifacts->SetVisible(gameplayHud);
        SetHudText(_hudArtifacts, "ARTIFACTS " + std::to_string(_artifactsCollected) + "/5", Color(0.82f, 0.84f, 0.76f, 1.f));
    }
    for (size_t i = 0; i < 5; i++) {
        if (!_hudInventory[i])
            continue;
        const bool collected = i < _artifacts.size() && _artifacts[i].collected;
        _hudInventory[i]->SetVisible(gameplayHud);
        SetHudText(_hudInventory[i],
                   std::string(collected ? "[X]  " : "[ ]  ") + _artifacts[i].name,
                   collected ? Color(0.48f, 0.98f, 0.58f, 1.f) : Color(0.72f, 0.74f, 0.70f, 1.f));
    }
    if (_hudHealth) {
        _hudHealth->SetVisible(gameplayHud);
        int health = (int)std::ceil(_health);
        SetHudText(_hudHealth, "HEALTH " + std::to_string(health),
                   health > 60 ? Color(0.55f, 0.88f, 0.48f, 1.f) : (health > 25 ? Color(0.94f, 0.70f, 0.22f, 1.f) : Color(0.96f, 0.16f, 0.09f, 1.f)));
    }
    if (_hudStamina) {
        _hudStamina->SetVisible(gameplayHud);
        SetHudText(_hudStamina,
                   std::string(_isSprinting ? "SPRINTING " : "STAMINA ") + std::to_string((int)std::ceil(_stamina)),
                   _stamina < 25.f ? Color(0.96f, 0.24f, 0.15f, 1.f) : Color(0.92f, 0.76f, 0.34f, 1.f));
    }
    if (_hudFlashlight) {
        _hudFlashlight->SetVisible(gameplayHud);
        SetHudText(_hudFlashlight,
                   _flashlightEnabled ? "FLASHLIGHT ON [F]" : "FLASHLIGHT OFF [F]",
                   _flashlightEnabled ? Color(0.98f, 0.82f, 0.40f, 1.f) : Color(0.58f, 0.58f, 0.54f, 1.f));
    }
    if (_hudCrosshair) {
        _hudCrosshair->SetVisible(inPlay || inPause);
        if (_attackPunch > 0.35f)
            SetHudText(_hudCrosshair, "+", Color(0.98f, 0.22f, 0.12f, 1.f));
        else
            SetHudText(_hudCrosshair, "+", Color(0.94f, 0.70f, 0.25f, 1.f));
    }
    if (_hudMessage)
        _hudMessage->SetVisible(gameplayHud && _messageTimer > 0.f);
    if (_hudScareText || _hudScareEcho) {
        const bool showingScare = _exitScareTimer > 0.f;
        const float elapsed = 2.4f - _exitScareTimer;
        const char* scareLine = elapsed < 0.30f ? "IT GOT OUT"
            : (elapsed < 0.95f ? "DON'T TURN AROUND"
            : (elapsed < 1.65f ? "IT KNOWS YOUR FACE" : "STAY WITH US"));
        const float jitter = showingScare ? sinf(elapsed * 87.f) * 34.f : 0.f;
        const Color scareColor = sinf(elapsed * 48.f) > -0.15f
            ? Color(0.98f, 0.08f, 0.04f, 1.f) : Color(0.42f, 0.02f, 0.02f, 1.f);
        if (_hudScareText) {
            _hudScareText->SetVisible(showingScare);
            SetHudText(_hudScareText, scareLine, scareColor);
            _hudScareText->SetBounds(Rect(Vec2(220.f + jitter, 350.f), Vec2(1160.f, 110.f)));
            _hudScareText->SetRotation(showingScare ? sinf(elapsed * 53.f) * 2.8f : 0.f);
        }
        if (_hudScareEcho) {
            _hudScareEcho->SetVisible(showingScare);
            SetHudText(_hudScareEcho, elapsed < 1.2f ? "RUN" : "IT FOLLOWED YOU HOME", Color(0.96f, 0.76f, 0.69f, 1.f));
            _hudScareEcho->SetBounds(Rect(Vec2(300.f - jitter * 0.45f, 465.f + jitter * 0.2f), Vec2(1000.f, 56.f)));
            _hudScareEcho->SetRotation(showingScare ? -sinf(elapsed * 41.f) * 1.6f : 0.f);
        }
    }
    if (_hudBloodOverlay)
        _hudBloodOverlay->SetVisible(_state == GameState::Win && _exitScareTimer <= 0.f);
    if (_hudWarning)
        _hudWarning->SetVisible(inPause || (gameplayHud && _warningTimer > 0.f));
    if (_hudControls)
        _hudControls->SetVisible(inMenu || inPause);

    if (_hudWarning && inPause)
        SetHudText(_hudWarning, "PAUSED - ESC TO RESUME", Color(0.92f, 0.78f, 0.48f, 1.f));
}

void DemoLayer::SetSkybox() {
    auto* skyTexture = ResourceCache::Get()->GetResource<TextureResource>("NightSky");
    if (!skyTexture)
        return;

    auto* skyMaterial = new MaterialResource();
    skyMaterial->SetName("NightSkybox");
    skyMaterial->SetId(Guid(0, 0, 4, 900));
    skyMaterial->GetMaterial()->SetTemplate("default_skybox");

    const char* textureSlots[6] = { "Back", "Front", "Left", "Right", "Bottom", "Top" };
    for (const char* slot : textureSlots) {
        ResourceReference reference;
        reference.SetResource(skyTexture);
        skyMaterial->GetMaterial()->SetTexture(slot, reference);
    }

    skyMaterial->GetMaterial()->SetParameter("Tint color", Color(0.55f, 0.62f, 0.78f, 1.f));
    skyMaterial->GetMaterial()->SetParameter("Exposure", 0.62f);
    skyMaterial->SetState(RESOURCE_STATE_READY);
    ResourceCache::Get()->PushResource(skyMaterial);

    _scene->GetEnvironmentSettings()._skybox_material.SetResource(skyMaterial);
}

void DemoLayer::OnUpdate() {
    float dt = std::min(TimePerf::Get()->GetDeltaTime(), 0.05f);
    _attackCooldown = std::max(0.f, _attackCooldown - dt);
    _hurtCooldown = std::max(0.f, _hurtCooldown - dt);
    _attackPunch = std::max(0.f, _attackPunch - dt * 4.5f);
    _damageFlash = std::max(0.f, _damageFlash - dt * 1.8f);
    if (_exitScareTimer > 0.f) {
        _exitScareTimer = std::max(0.f, _exitScareTimer - dt);
        if (_exitScreamer) {
            float approach = 1.f - _exitScareTimer / 2.4f;
            Vec3 forward = _aimForward.GetNormalized();
            float distance = 1.65f - 1.27f * approach;
            _exitScreamer->SetPosition(_camera->GetPosition() + forward * distance);
            _exitScreamer->SetRotation(RotationFacing(forward * -1.f));
            float pulse = 0.92f + 0.8f * approach + 0.10f * sinf(_elapsed * 75.f);
            _exitScreamer->SetScale(Vec3(pulse));
            if (_exitScareTimer <= 0.f)
                _exitScreamer->SetActive(false);
        }
    }
    _messageTimer = std::max(0.f, _messageTimer - dt);
    _warningTimer = (_state == GameState::Pause) ? 999.f : std::max(0.f, _warningTimer - dt);
    _lockedDoorMessageCooldown = std::max(0.f, _lockedDoorMessageCooldown - dt);
    _scareCooldown = std::max(0.f, _scareCooldown - dt);
    _playerNoiseTimer = std::max(0.f, _playerNoiseTimer - dt);

    Input* input = Input::Get();

    if (input->IsKeyPressed(KEY_CODE_ESCAPE))
        TogglePause();

    if (input->IsKeyPressed(KEY_CODE_R) && (_state == GameState::Play || _state == GameState::Win || _state == GameState::Lose || _state == GameState::Pause))
        ResetGame();

    if (input->IsKeyPressed(KEY_CODE_ENTER))
        StartGame();

    if (_state == GameState::Play && input->IsKeyPressed(KEY_CODE_F)) {
        _flashlightEnabled = !_flashlightEnabled;
        if (_flashlightLight)
            _flashlightLight->GetIntensity() = _flashlightEnabled ? 9.0f : 0.f;
        if (_flashlightLens)
            _flashlightLens->SetActive(_flashlightEnabled);
        PlaySfx("flashlight");
        _playerNoiseRadius = 3.5f;
        _playerNoiseTimer = 0.35f;
        SetMessage(_flashlightEnabled ? "FLASHLIGHT ON" : "FLASHLIGHT OFF", 0.8f);
    }

    if (input->IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || input->IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || input->IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
        if (_state == GameState::Menu)
            StartGame();
        else if (_state == GameState::Win || _state == GameState::Lose) {
            ResetGame();
            StartGame();
        }
    }

    const bool attackPressed = _state == GameState::Play
        && (input->IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || input->IsKeyPressed(KEY_CODE_SPACE));

    const bool wantMouseCapture = (_state == GameState::Play);
    if (wantMouseCapture != _mouseCaptured) {
        SDL_SetRelativeMouseMode(wantMouseCapture ? SDL_TRUE : SDL_FALSE);
        SDL_ShowCursor(wantMouseCapture ? SDL_DISABLE : SDL_ENABLE);
        _mouseCaptured = wantMouseCapture;
        if (wantMouseCapture) {
            input->ClearMouseCursorDelta();
            _ignoreMouseDelta = true;
        }
    }

    UpdatePlayerCamera(dt);
    if (attackPressed)
        Attack();
    UpdateMonsters(dt);
    UpdateArtifacts(dt);
    UpdateTorchFlicker(dt);
    CheckExit();

    if (_hudMessage && _messageTimer <= 0.f && _state != GameState::Win && _state != GameState::Lose)
        _hudMessage->SetVisible(false);

    UpdateHUD();

    IRenderer* renderer = Application::Get()->GetRenderer();
    if (renderer)
        renderer->QueueSkyProbeUpdate();
}
