#pragma once

#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanSampler.hpp>
#include <imgui.h>
#include <imgui_impl_vulkan.h>

class ImguiVulkanTexture {
public:
	ImTextureID imtexture;
	VSGE::VulkanTexture texture;

	void CreateImgui(VSGE::VulkanSampler& sampler) {
		imtexture = ImGui_ImplVulkan_AddTexture(sampler.GetSampler(), texture.GetImageView(), VK_IMAGE_LAYOUT_UNDEFINED);
	}

	void CreateFromFile(std::string fpath, VSGE::VulkanSampler& sampler) {
		texture.CreateFromFile(fpath);
		CreateImgui(sampler);
	}
};