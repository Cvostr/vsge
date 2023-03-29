#pragma once

#include <string>

namespace Mpi {
#ifdef _WIN32
	std::wstring ConvertUtf8ToWide(const std::string& str);

	std::string ConvertWideToUtf8(const std::wstring& wstr);

	
#endif

	void setTerminalUtf8();

	int getch();
}