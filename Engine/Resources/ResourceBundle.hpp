#pragma once

#include <string>
#include <vector>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	class BundleEntry {
	public:
		std::string name;
		uint32 offset;
		uint32 size;
	};

	class ResourceBundle {
	private:
		std::vector<BundleEntry> entries;
	public:
		void CreateFromFile(const std::string& file);
		BundleEntry* GetEntry(const std::string& name);
		uint32 GetEntriesCount() { return static_cast<uint32>(entries.size()); }
	};
}