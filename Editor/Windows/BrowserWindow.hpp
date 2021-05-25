#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vector>
#include <string>
#include "../Base/ImVulkanTexture.hpp"
#include <Resources/Resource.hpp>

namespace VSGEditor {

	struct FileEntry {
		bool isDir;
		std::string abs_path; //full path on disk
		std::string rel_path;
		std::string name; //name of file with ext
		std::string ext; //file extension
		std::string directory; //directory path of file
		bool is3dModel() {
			return !ext.compare(".rp3m");
		}
		bool isTexture() {
			return !ext.compare(".DDS") || !ext.compare(".dds") || !ext.compare(".PNG") || !ext.compare(".png");
		}
		bool is3dWorld() {
			return !ext.compare(".scn");
		}
		bool isMaterial() {
			return !ext.compare(".rpmat");
		}
	};

	typedef std::vector<FileEntry> tFileEntryList;
	typedef std::pair<std::string, ImguiVulkanTexture*> STRIMGVKT;

	class FileBrowserWindow : public EditorWindow {
	private:
		tFileEntryList mFiles;
		std::string mCurrentDir;
		std::string mRootDir;

		std::string new_file_name;
		FileEntry* entryToDelete;

		VSGE::VulkanSampler sampler;
		struct {
			ImguiVulkanTexture mBackBtnIcon;
			ImguiVulkanTexture mDirIcon;
			ImguiVulkanTexture mUnknownFile;
			ImguiVulkanTexture mEmptyTextureIcon;
			ImguiVulkanTexture m3DModelIcon;
			ImguiVulkanTexture mSceneIcon;
		}FileIcons;
		
		std::vector<STRIMGVKT*> mTextureResources;

		void DeleteFileDialog(FileEntry* Entry);
		void RenameFileDialog(FileEntry* Entry);
		VSGE::Resource* GetResourceWithFilePath(const std::string& fpath);
	public:

		FileBrowserWindow(std::string RootDir);

		ImguiVulkanTexture* GetTextureResource(const std::string& fname);
		ImguiVulkanTexture* GetCheckerboardTexture() { return &FileIcons.mEmptyTextureIcon; }

		

		void OnDrawWindow();

		void Regroup(uint32 width, uint32 height);

		void cd_up();
		void cd(std::string DirName);
		void SetDirectory(std::string Dir);
		void UpdateDirectoryContent();
		void OpenFile(const FileEntry& Entry);

		const std::string& GetCurrentDir() {
			return mCurrentDir;
		}
	};
}