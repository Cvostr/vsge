#pragma once

#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanSampler.hpp>
#include <imgui.h>
#include <imgui_impl_vulkan.h>

class ImguiVulkanTexture {
public:
	ImTextureID imtexture;
	VSGE::VulkanTexture* texture;

	void CreateImgui(VSGE::VulkanSampler& sampler) {
		imtexture = ImGui_ImplVulkan_AddTexture(sampler.GetSampler(), texture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void CreateFromFile(std::string fpath, VSGE::VulkanSampler& sampler) {
		texture = new VSGE::VulkanTexture;
		texture->CreateFromFile(fpath);
		CreateImgui(sampler);
	}

	ImguiVulkanTexture() {
		texture = nullptr;
		imtexture = nullptr;
	}

	~ImguiVulkanTexture() {
		SAFE_RELEASE(texture);
	}
};