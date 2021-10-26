#include "InspectorResourceTypes.hpp"
#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Misc/Thumbnails.hpp>
#include <imgui.h>

using namespace VSGE;

void VSGEditor::DrawTextureResourceUI(VSGE::Resource* resource) {
	Texture* texture = ((TextureResource*)resource)->GetTexture();

	std::string resolution_text = "Max Resolution - " + std::to_string(texture->GetWidth()) + " x " + std::to_string(texture->GetHeight());
	ImGui::Text(resolution_text.c_str());

	std::string mips_text = "Mips count - " + std::to_string(texture->GetMipsCount());
	ImGui::Text(mips_text.c_str());

	TextureFormat format = texture->GetFormat();
	std::string format_str = "";
	switch (format) {
	case FORMAT_RGBA:
		format_str = "R8G8B8A8";
		break;
	case FORMAT_BC1_UNORM:
		format_str = "BC1 UNORM";
		break;
	case FORMAT_BC2_UNORM:
		format_str = "BC2 UNORM";
		break;
	case FORMAT_BC3_UNORM:
		format_str = "BC3 UNORM";
		break;
	}

	std::string format_text = "Format - " + format_str;
	ImGui::Text(format_text.c_str());

	ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
	if(tex)
		ImGui::Image(tex->imtexture, ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowWidth()));
}