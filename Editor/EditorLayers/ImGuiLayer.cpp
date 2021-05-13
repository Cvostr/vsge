#include "ImGuiLayer.hpp"
#include "EditorLayer.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Engine/Application.hpp>
#include <Engine/Window.hpp>

#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

using namespace VSGEditor;

ImGuiLayer* ImGuiLayer::_this = nullptr;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void ImGuiLayer::OnAttach() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::Window* win = &app->GetWindow();
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VSGE::VulkanDevice* device = vk->GetDevice();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.DisplaySize = ImVec2((float)win->GetWindowWidth(), (float)win->GetWindowWidth());
    //io.DisplayFramebufferScale = ImVec2(1.25f, 1.25f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    win->SetResizeable(true);

    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    imgui_pool.SetPoolSizes(pool_sizes, 11);
    imgui_pool.SetDescriptorSetsCount(1000 * 11);
    imgui_pool.Create();

    imgui_rpass.PushColorOutputAttachment();
    imgui_rpass.Create();

    imgui_fb.PushOutputAttachment(0);
    imgui_fb.Create(&imgui_rpass);

    imageAvailable.Create();
    presentBegin.Create();

    cmdpool.Create(device->GetGraphicsQueueFamilyIndex());
    cmdbuf.Create(&cmdpool);

    ImGui_ImplSDL2_InitForVulkan(win->GetSdlWinPtr());
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vk->GetInstance()->GetInstance();
    init_info.PhysicalDevice = device->getPhysicalDevice();
    init_info.Device = device->getVkDevice();
    init_info.QueueFamily = device->GetGraphicsQueueFamilyIndex();
    init_info.Queue = device->GetGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = imgui_pool.GetDescriptorPool();
    init_info.Allocator = NULL;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;

    ImGui_ImplVulkan_Init(&init_info, imgui_rpass.GetRenderPass());
#ifdef _WIN32
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
#endif
    cmdbuf.Begin();
    ImGui_ImplVulkan_CreateFontsTexture(cmdbuf.GetCommandBuffer());
    cmdbuf.End();
    VulkanGraphicsSubmit(cmdbuf);
   
}

void ImGuiLayer::VulkanRecordCmdBuf(ImDrawData* draw_data) {
    cmdbuf.Begin();
    imgui_rpass.CmdBegin(cmdbuf, imgui_fb);

    ImGui_ImplVulkan_RenderDrawData(draw_data, cmdbuf.GetCommandBuffer());

    cmdbuf.EndRenderPass();
    cmdbuf.End();
}

void ImGuiLayer::VulkanRender(ImDrawData* draw_data, VSGE::VulkanSemaphore* endSemaphore) {
    VulkanRecordCmdBuf(draw_data);

    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VSGE::Window* win = &VSGE::Application::Get()->GetWindow();

    uint32_t _imageIndex;

    VkResult imageResult = AcquireNextImage(imageAvailable, _imageIndex);

    _imageIndex = 0;

    //Check, if swapchain is no more suitable
    if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
        //Swapchain is no more suitable
        vkDeviceWaitIdle(vk->GetDevice()->getVkDevice());
        //Destroy output framebuffer
        imgui_fb.Destroy();
        imgui_fb.SetSize(win->GetWindowWidth(), win->GetWindowHeight());
        imgui_rpass.SetClearSize(win->GetWindowWidth(), win->GetWindowHeight());
        //Recreate swapchain
        vk->GetSwapChain()->Destroy();
        vk->GetSwapChain()->initSwapchain(vk->GetDevice());

        imgui_fb.PushOutputAttachment(0);
        imgui_fb.Create(&imgui_rpass);

        VulkanRecordCmdBuf(draw_data);
    }

    VulkanGraphicsSubmit(cmdbuf, imageAvailable, *endSemaphore);

}


void ImGuiLayer::OnUpdate() {
    EditorLayer* editor_layer = EditorLayer::Get();
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::Window* win = &app->GetWindow();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(win->GetSdlWinPtr());
    ImGui::NewFrame();


	ImGui::BeginMainMenuBar();

    for (auto menu : mMenus) {
        menu->OnDrawMenu();
    }

	ImGui::EndMainMenuBar();


    for (uint32 win_i = 0; win_i < mWindows.size(); win_i ++) {
        EditorWindow* win = mWindows[win_i];
        win->OnDrawWindow();
    }


    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();

    VulkanRender(draw_data, renderer->GetBeginSemaphore());

    renderer->DrawScene(editor_layer->GetCamera());

    VulkanPresent(*renderer->GetEndSemaphore(), 0);

}

void ImGuiLayer::OnDetach() {
    cmdbuf.Destroy();
    cmdpool.Destroy();

    presentBegin.Destroy();
    imageAvailable.Destroy();

    imgui_fb.Destroy();
    imgui_rpass.Destroy();
    imgui_pool.Destroy();


    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnSDL2Event(void* event_ptr) {
    SDL_Event* event = static_cast<SDL_Event*>(event_ptr);
    ImGui_ImplSDL2_ProcessEvent(event);
}

void ImGuiLayer::OnEvent(const VSGE::IEvent& event) {
    VSGE::DispatchEvent<VSGE::EventWindowResized>(event, EVENT_FUNC(ImGuiLayer::OnWindowResize));
}

void ImGuiLayer::OnWindowResize(const VSGE::EventWindowResized& wr) {
    int oldWidth = wr.GetOldWidth();
    int oldHeight = wr.GetOldHeight();

    int width = wr.GetWidth();
    int height = wr.GetHeight();

    float relX = (float)width / oldWidth;
    float relY = (float)height / oldHeight;

    for (auto window : mWindows) {
        ImVec2 old_pos = window->GetPos();
        ImVec2 old_size = window->GetSize();

        window->SetSize((uint32)(old_size.x * relX), (uint32)(old_size.y * relY));
        window->SetPos((uint32)(old_pos.x * relX), (uint32)(old_pos.y * relY));
    }
}

void ImGuiLayer::AddWindow(EditorWindow* window) {
    mWindows.push_back(window);
}

void ImGuiLayer::RemoveWindow(EditorWindow* window) {
    tEditorWindowList::iterator it = std::remove(mWindows.begin(), mWindows.end(), window);
    mWindows.pop_back();
}

void ImGuiLayer::AddMenu(ImGuiMenu* menu) {
    mMenus.push_back(menu);
}