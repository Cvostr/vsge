#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	bool LoadFile(const std::string& FilePath, char** out, uint32 *Size = nullptr, bool Terminate = false);
	bool LoadFilePartially(const std::string& FilePath, char** out, uint32 Offset, uint32 Size);
}