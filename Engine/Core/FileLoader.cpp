#include "FileLoader.hpp"
#include "Logger.hpp"

#include "stdio.h"

bool VSGE::LoadFile(const std::string& FilePath, char** out, size_t* Size, bool Terminate) {
	FILE* pFile = nullptr;
	//Try to open file with read binary
	fopen_s(&pFile, FilePath.c_str(), "rb");

	if (pFile == NULL) {
		//Logger::Log("Error opening file " + FilePath, LogType::LOG_TYPE_ERROR);
		return false;
	}
	//Seek to end
	fseek(pFile, 0, SEEK_END);
	size_t FileSize = ftell(pFile);
	if (Terminate)
		FileSize++;
	//Seek to begin
	rewind(pFile);
	//Allocate memory
	*out = new char[FileSize];
	fread(*out, 1, FileSize, pFile);
	//Close file stream
	fclose(pFile);

	if (Terminate)
		(*out)[FileSize - 1] = '\0';

	if (Size != nullptr)
		*Size = FileSize;

	return true;
}

bool VSGE::LoadFilePartially(const std::string& FilePath, char** out, size_t Offset, size_t Size) {
	FILE* pFile = nullptr;
	//Try to open file with read binary
	fopen_s(&pFile, FilePath.c_str(), "rb");

	if (pFile == NULL) {
		//Logger::Log("Error opening file " + FilePath, LogType::LOG_TYPE_ERROR);
		return false;
	}
	//Seek to end
	fseek(pFile, Offset, SEEK_SET);

	//Seek to begin
	rewind(pFile);
	//Allocate memory
	*out = new char[Size];
	fread(*out, 1, Size, pFile);
	//Close file stream
	fclose(pFile);

	return true;
}