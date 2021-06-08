#include "ConsoleWindow.hpp"

using namespace VSGEditor;

void ConsoleWindow::OnDrawWindow() {
	if (Draw("Console")) {
		ImGui::End();
	}
}