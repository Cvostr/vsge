#pragma once

#include "../Base/ImVulkanTexture.hpp"

namespace VSGEditor {
	class EditorIcons {
	private:
		ImguiVulkanTexture _backBtnIcon;
		ImguiVulkanTexture _dirIcon;
		ImguiVulkanTexture _unknownFile;
		ImguiVulkanTexture _3DModelIcon;
		ImguiVulkanTexture _sceneIcon;
		ImguiVulkanTexture _scriptIcon;
		ImguiVulkanTexture _audioIcon;
		ImguiVulkanTexture _playIcon;
		ImguiVulkanTexture _pauseIcon;
		ImguiVulkanTexture _stopIcon;
		ImguiVulkanTexture _translateIcon;
		ImguiVulkanTexture _scaleIcon;
		ImguiVulkanTexture _rotateIcon;

		VSGE::VulkanSampler _sampler;

		static EditorIcons* _this;
	public:

		EditorIcons();
		~EditorIcons();

		static EditorIcons* Get() {
			return _this;
		}
		void LoadIcons();
		void FreeIcons();

		void* GetUnknownFileIcon();

		void* GetDirIcon();

		void* GetBackBtnIcon();

		void* GetSceneIcon();

		void* Get3DModelIcon();

		void* GetScriptIcon();

		void* GetAudioIcon();

		void* GetPlayIcon();

		void* GetPauseIcon();

		void* GetStopIcon();

		void* GetTranslateIcon();

		void* GetScaleIcon();

		void* GetRotateIcon();
	};
}