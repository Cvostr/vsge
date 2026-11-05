#include <cstdio>
#include <string>
#ifdef _WIN32
extern "C" {
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void* hModule, char* lpFilename, unsigned long nSize);
__declspec(dllimport) int __stdcall SetCurrentDirectoryA(const char* lpPathName);
}
#endif
#include <Engine/Application.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Physics/PhysicsLayer.hpp>
#include <Scene/SceneLayer.hpp>
#include <Resources/ResourceLayer.hpp>
#include <Resources/DefaultResources.hpp>
#include <Audio/AudioLayer.hpp>
#include <Input/InputLayer.hpp>
#include <UI/UiLayer.hpp>
#include <Graphics/Fonts.hpp>
#include <Layers/Graphics/VulkanLayer.hpp>
#include "Graphics/Vulkan/VulkanRAPI.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Graphics/FontManager.hpp>
#include <System/PlatformSpecific.hpp>
#include "Layers/DemoLayer.hpp"
namespace {
std::string GetExecutableDirectory() {
#if defined(_WIN32)
    char path[260] = {};
    unsigned long len = GetModuleFileNameA(nullptr, path, sizeof(path));
    if (len == 0 || len == sizeof(path)) return ".";
    std::string filePath(path, len);
    size_t pos = filePath.find_last_of("\\/");
    if (pos == std::string::npos) return ".";
    return filePath.substr(0, pos);
#else
    return ".";
#endif
}
}

using namespace VSGE;
Application* VSGEMain(int argc, char* argv[]) {
    const std::string executableDirectory = GetExecutableDirectory();
#if defined(_WIN32)
    SetCurrentDirectoryA(executableDirectory.c_str());
#endif
    freopen("HouseOfShadows.log", "w", stdout);
    freopen("HouseOfShadows.log", "a", stderr);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    ApplicationCreateInfo descr = { "House of Shadows", 1 };
    descr.graphicsApi = GRAPHICS_API_VULKAN;
    descr.headless = false;
    descr.server = false;
    descr.application_dir = executableDirectory;
    auto app = new Application(descr);
    app->GetWindow().CreateWindow(1600, 900, "House of Shadows");
    PhysicsLayer* phys_layer = new PhysicsLayer;
    MonoScriptingLayer* mono_scripting_layer = new MonoScriptingLayer;
    SceneLayer* scene_layer = new SceneLayer;
    UiLayer* uiLayer = new UiLayer;  // Create early so UiLayer::Get() works in DemoLayer::OnAttach
    GlyphManager* glyph_manager = new GlyphManager;
    app->AddLayer(mono_scripting_layer);
    VulkanRAPI* vk = new VulkanRAPI;
    vk->Create(&app->GetWindow(), false);
    VulkanRenderer* renderer = new VulkanRenderer;
    renderer->GetFinalPass()->SetRenderToSwapchain(true);
    renderer->GetFinalPass()->Create();
    app->SetRenderer(renderer);
#if defined(_WIN32)
    glyph_manager->AddFontContainer("c:\\Windows\\Fonts\\Arial.ttf", "arial");
#elif defined(__APPLE__)
    glyph_manager->AddFontContainer("/System/Library/Fonts/Supplemental/Arial.ttf", "arial");
    glyph_manager->AddFontContainer("/System/Library/Fonts/Helvetica.ttc", "arial");
#else
    glyph_manager->AddFontContainer("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", "arial");
    glyph_manager->AddFontContainer("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", "arial");
#endif
    AddDefaultMaterial();
    AddDefaultMeshes();
    app->AddLayer(new AudioLayer);
    // Reorder for correct physics update order: DemoLayer -> PhysicsLayer -> SceneLayer
    app->AddLayer(new DemoLayer);
    app->AddLayer(phys_layer);
    app->AddLayer(scene_layer);
    app->AddLayer(new ResourceLayer);
    app->AddLayer(new InputLayer);
    app->AddLayer(new VulkanLayer);
    app->AddLayer(uiLayer);
    return app;
}
