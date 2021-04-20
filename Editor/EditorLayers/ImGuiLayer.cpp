#include "ImGuiLayer.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Engine/Application.hpp>

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
    //io.DisplayFramebufferScale = ImVec2(1.25f, 1.25f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

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

    imgui_fb.SetSize(1280, 720);
    imgui_fb.PushOutputAttachment(0);
    imgui_fb.Create(&imgui_rpass);

    imageAvailable.Create();
    presentBegin.Create();

    cmdpool.Create(device->GetGraphicsQueueFamilyIndex());
    cmdbuf.Create(cmdpool);

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
   // init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, imgui_rpass.GetRenderPass());

    cmdbuf.Begin();
    ImGui_ImplVulkan_CreateFontsTexture(cmdbuf.GetCommandBuffer());
    cmdbuf.End();
    VulkanGraphicsSubmit(cmdbuf);
   
}

void ImGuiLayer::VulkanComputeAndPresent(ImDrawData* draw_data) {
    cmdbuf.Begin();
    imgui_rpass.CmdBegin(cmdbuf, imgui_fb);

    ImGui_ImplVulkan_RenderDrawData(draw_data, cmdbuf.GetCommandBuffer());

    cmdbuf.EndRenderPass();
    cmdbuf.End();

    VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();

    uint32_t _imageIndex;
    VkResult imageResult = vkAcquireNextImageKHR(vk->GetDevice()->getVkDevice(),
        vk->GetSwapChain()->GetSwapChain(), UINT64_MAX, imageAvailable.GetSemaphore(), VK_NULL_HANDLE, &_imageIndex);

    _imageIndex = 0;

    if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
        //this->PresentInfrastructure.RecreateSwapchain();
        //_imageIndex = 0;
        //return;
    }

    VulkanGraphicsSubmit(cmdbuf, imageAvailable, presentBegin);

    VulkanPresent(presentBegin, _imageIndex);

}

void ImGuiLayer::OnUpdate() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::Window* win = &app->GetWindow();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(win->GetSdlWinPtr());
    ImGui::NewFrame();


	ImGui::BeginMainMenuBar();

    for (auto menu : mMenus) {
        menu->OnDrawMenu();
    }

    if (ImGui::BeginMenu("World"))
    {
        if (ImGui::MenuItem("Create Mesh Object", NULL)) {
        }
		
		ImGui::EndMenu();
	}
	

	ImGui::EndMainMenuBar();


    for (auto window : mWindows) {
        window->OnDrawWindow();
    }


    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    VulkanComputeAndPresent(draw_data);
}

void ImGuiLayer::OnDetach() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnSDL2Event(void* event_ptr) {
    SDL_Event* event = static_cast<SDL_Event*>(event_ptr);
    ImGui_ImplSDL2_ProcessEvent(event);
}

void ImGuiLayer::AddWindow(EditorWindow* window) {
    mWindows.push_back(window);
}
void ImGuiLayer::AddMenu(ImGuiMenu* menu) {
    mMenus.push_back(menu);
}