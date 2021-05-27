#include "Windows_Menu.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

void Windows_Menu::OnDrawMenu() {
	if (ImGui::BeginMenu("Windows"))
	{

		ImGui::EndMenu();
	}
}