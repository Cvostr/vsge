#include "ImGuiLayer.hpp"
#include "EditorLayer.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Engine/Application.hpp>
#include <Engine/Window.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>
#include <Misc/VkMaterialsThumbnails.hpp>

using namespace VSGEditor;

ImGuiLayer* ImGuiLayer::_this = nullptr;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
const float k_roundness = 2.0f;

static void ImGui_ApplyStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.ScrollbarSize = 20.0f;
    style.FramePadding = ImVec2(5, 5);
    style.ItemSpacing = ImVec2(6, 5);
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.WindowRounding = k_roundness;
    style.FrameRounding = k_roundness;
    style.PopupRounding = k_roundness;
    style.GrabRounding = k_roundness;
    style.ScrollbarRounding = k_roundness;
    style.Alpha = 1.0f;
}

ImGuiLayer::ImGuiLayer() {
    _this = this;
    presenter = nullptr;
    _draw_windows = true;
}

void ImGuiLayer::OnAttach() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::Window* win = &app->GetWindow();
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VSGE::VulkanDevice* device = vk->GetDevice();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize = ImVec2((float)win->GetWindowWidth(), (float)win->GetWindowWidth());
    //io.DisplayFramebufferScale = ImVec2(1.25f, 1.25f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ApplyStyle();

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

    imgui_pool.SetPoolSizes(pool_sizes, 5);
    imgui_pool.SetDescriptorSetsCount(1000 * 5);
    imgui_pool.Create();

    presenter = new VSGE::VulkanPresenter;

    imageAvailable.Create();
    presentBegin.Create();

    cmdpool.Create(device->GetGenericQueueFamilyIndex());
    cmdbuf.Create(&cmdpool);

    ImGui_ImplSDL2_InitForVulkan(win->GetSdlWinPtr());
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vk->GetInstance()->GetInstance();
    init_info.PhysicalDevice = device->getPhysicalDevice();
    init_info.Device = device->getVkDevice();
    init_info.QueueFamily = device->GetGenericQueueFamilyIndex();
    init_info.Queue = device->GetGenericQueue()->GetQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = imgui_pool.GetDescriptorPool();
    init_info.Allocator = NULL;
    init_info.MinImageCount = vk->GetSwapChain()->GetSwapChainImagesCount();
    init_info.ImageCount = vk->GetSwapChain()->GetSwapChainImagesCount();

    ImGui_ImplVulkan_Init(&init_info, presenter->GetRenderPass()->GetRenderPass());
#ifdef _WIN32
    AddImGuiFont("c:\\Windows\\Fonts\\Arial.ttf");
#endif
#ifdef __linux__
    AddImGuiFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
#endif
    cmdbuf.Begin();
    ImGui_ImplVulkan_CreateFontsTexture(cmdbuf.GetCommandBuffer());
    cmdbuf.End();
    VulkanGraphicsSubmit(cmdbuf);

    mHoldOnWindow = new HoldOnWindow;
    mHoldOnWindow->Hide();
}

void ImGuiLayer::AddImGuiFont(const std::string& path, float size) {
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0x0E00, 0x0E7F, // Thai
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD7A3, // Korean characters
        0,
    };

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(path.c_str(), size, NULL, ranges);
}

void ImGuiLayer::VulkanRecordCmdBuf(ImDrawData* draw_data, uint32 image_index) {
    cmdbuf.Begin();
    presenter->GetRenderPass()->CmdBegin(cmdbuf, *presenter->GetFramebuffer(image_index));

    ImGui_ImplVulkan_RenderDrawData(draw_data, cmdbuf.GetCommandBuffer());

    cmdbuf.EndRenderPass();

    VkMaterialsThumbnails::Get()->RecordCmdBuffer(&cmdbuf);

    cmdbuf.End();
}

uint32 ImGuiLayer::VulkanRender(ImDrawData* draw_data, VSGE::VulkanSemaphore* endSemaphore) {
    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
    VSGE::Window* win = &VSGE::Application::Get()->GetWindow();

    uint32_t imageIndex;
    VkResult imageResult = AcquireNextImage(imageAvailable, imageIndex);

    presenter->Update(imageResult);

    if (!presenter->IsRecreated()) {
        VulkanRecordCmdBuf(draw_data, imageIndex);
        VulkanGraphicsSubmit(cmdbuf, imageAvailable, *endSemaphore);
    }

    return imageIndex;
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

    DrawDockWindow();
    
    if (_draw_windows) {
        for (uint32 win_i = 0; win_i < mWindows.size(); win_i++) {
            EditorWindow* win = mWindows[win_i];
            win->OnDrawWindow();
        }
    }
    ImGui::End();
    mHoldOnWindow->OnDrawWindow();

    ImGui::Render();
    
    ImDrawData* draw_data = ImGui::GetDrawData();

    VSGE::VulkanRenderer* renderer = VSGE::VulkanRenderer::Get();

    uint32 img_index = VulkanRender(draw_data, renderer->GetBeginSemaphore());

    if(presenter->IsRecreated()){
       return;
    } 

    VSGE::Camera* cam = nullptr;
    if (EditorLayer::Get()->GetEditorCameraMode() == EDITOR_CAMERA_MODE_EDIT_CAMERA) {
        cam = editor_layer->GetCamera();
    }

    renderer->DrawScene(cam);

    VulkanPresent(*renderer->GetEndSemaphore(), img_index);
}

void ImGuiLayer::OnDetach() {
    cmdbuf.Destroy();
    cmdpool.Destroy();

    presentBegin.Destroy();
    imageAvailable.Destroy();

    delete presenter;
    delete mHoldOnWindow;
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
    
}

void ImGuiLayer::AddWindow(EditorWindow* window) {
    mWindows.push_back(window);
}

void ImGuiLayer::RemoveWindow(EditorWindow* window) {
    tEditorWindowList::iterator it = std::remove(mWindows.begin(), mWindows.end(), window);
    mWindows.pop_back();
}

void ImGuiLayer::SetDrawWindows(bool draw_windows) {
    _draw_windows = draw_windows;
}

HoldOnWindow* ImGuiLayer::GetHoldOnWindow() {
    return mHoldOnWindow;
}

void ImGuiLayer::AddMenu(ImGuiMenu* menu) {
    mMenus.push_back(menu);
}

void ImGuiLayer::RegroupWindows() {
    VSGE::Window* win = &VSGE::Application::Get()->GetWindow();
    for (auto window : mWindows) {
        window->Regroup(win->GetWindowWidth(), win->GetWindowHeight());
    }
}

void ImGuiLayer::SetTabWindowCurrent(const std::string& window_title) {
    ImGuiWindow* window = ImGui::FindWindowByName(window_title.c_str());
    if (window == NULL || window->DockNode == NULL || window->DockNode->TabBar == NULL)
        return;
    window->DockNode->TabBar->NextSelectedTabId = window->ID;
}

void ImGuiLayer::DrawDockWindow() {
    const auto window_flags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    // Set window position and size
    float offset_y = TOOLBAR_WINDOW_HEIGHT;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset_y));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset_y));
    ImGui::SetNextWindowViewport(viewport->ID);

    // Set window style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    // Begin window
    std::string name = "##dock_window";
    bool open = true;
    bool m_editor_begun = ImGui::Begin(name.c_str(), &open, window_flags);
    ImGui::PopStyleVar(3);

    // Begin dock space
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable && m_editor_begun)
    {
        // Dock space
        const auto window_id = ImGui::GetID(name.c_str());
        if (!ImGui::DockBuilderGetNode(window_id))
        {
            // Reset current docking state
            ImGui::DockBuilderRemoveNode(window_id);
            ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
            ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

            ImGuiID dock_main_id = window_id;
            ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.2f, nullptr, &dock_main_id);
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
            ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
            const ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.6f, nullptr, &dock_right_id);
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);
            const ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

            // Dock windows
            ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_left_id);
            ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
            ImGui::DockBuilderDockWindow("Console", dock_down_id);
            ImGui::DockBuilderDockWindow("File Browser", dock_down_id);
            ImGui::DockBuilderDockWindow("World view", dock_main_id);
            //ImGui::DockBuilderDockWindow("Player view", dock_main_id);

            ImGui::DockBuilderFinish(dock_main_id);
        }

        ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }
}