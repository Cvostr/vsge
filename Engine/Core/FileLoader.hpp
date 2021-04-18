#pragma once

#include <string>

namespace VSGE {
	bool LoadFile(const std::string& FilePath, char** out, size_t *Size = nullptr, bool Terminate = false);
	bool LoadFilePartially(const std::string& FilePath, char** out, size_t Offset, size_t Size);
}