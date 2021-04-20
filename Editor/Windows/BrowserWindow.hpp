#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vector>
#include <string>

namespace VSGEditor {

	struct FileEntry {
		bool isDir;
		std::string abs_path;
		std::string rel_path;
		std::string name;
		std::string ext;
		bool is3dModel() {
			return !ext.compare(".rp3m");
		}
		bool isTexture() {
			return !ext.compare(".DDS") || !ext.compare(".dds");
		}
		bool is3dWorld() {
			return !ext.compare(".scn");
		}
		bool isMaterial() {
			return !ext.compare(".rpmat");
		}
	};

	typedef std::vector<FileEntry> tFileEntryList;

	class FileBrowserWindow : public EditorWindow {
	private:
		tFileEntryList mFiles;
		std::string mCurrentDir;
		std::string mRootDir;
	public:

		FileBrowserWindow(std::string RootDir);

		void OnDrawWindow();

		void cd_up();
		void cd(std::string DirName);
		void SetDirectory(std::string Dir);
		void UpdateDirectoryContent();
		void OpenFile(const FileEntry& Entry);
		void DeleteFile(const FileEntry& Entry);
	};
}