#include "ImGuiLayer.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <Graphics/OpenGL/OpenGL_RAPI.hpp>
#include <Engine/Application.hpp>
#include <GL/glew.h>

using namespace VSGEditor;

ImGuiLayer* ImGuiLayer::_this = nullptr;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void ImGuiLayer::OnAttach() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::Window* win = &app->GetWindow();
    VSGE::OpenGL_RAPI* ogl = VSGE::OpenGL_RAPI::Get();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.DisplayFramebufferScale = ImVec2(1.25f, 1.25f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(win->GetSdlWinPtr(), ogl->GetSDLContext());
    ImGui_ImplOpenGL3_Init();
}
void ImGuiLayer::OnUpdate() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::Window* win = &app->GetWindow();

    ImGui_ImplOpenGL3_NewFrame();
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
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(win->GetSdlWinPtr());
}

void ImGuiLayer::OnDetach() {
    ImGui_ImplOpenGL3_Shutdown();
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