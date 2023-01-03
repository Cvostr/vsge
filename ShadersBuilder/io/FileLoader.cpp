#include "FileLoader.hpp"
#include "stdio.h"
#include <cstring>

bool LoadFile(const std::string& FilePath, char** out, uint32* Size, bool Terminate) {
	//Try to open file with read binary
	FILE* pFile = fopen(FilePath.c_str(), "rb");

	if (pFile == NULL) {
		//Logger::Log("Error opening file " + FilePath, LogType::LOG_TYPE_ERROR);
		return false;
	}
	//Seek to end
	fseek(pFile, 0, SEEK_END);
	uint32 FileSize = ftell(pFile);
	if (Terminate)
		FileSize++;
	//Seek to begin
	rewind(pFile);
	//Allocate memory
	*out = new char[FileSize];
	memset(*out, 0, FileSize);
	fread(*out, 1, FileSize, pFile);
	//Close file stream
	fclose(pFile);

	if (Terminate)
		(*out)[FileSize - 1] = '\0';

	if (Size != nullptr)
		*Size = FileSize;

	return true;
}

bool LoadFilePartially(const std::string& FilePath, char** out, uint32 Offset, uint32 Size) {
	//Try to open file with read binary
	FILE* pFile = fopen(FilePath.c_str(), "rb");
	
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
