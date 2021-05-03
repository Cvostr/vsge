#include "StringHash.hpp"

using namespace VSGE;

void StringHash::Hash(const std::string& str) {
	hash = 0;
	for (uint32 i = 0; i < str.size(); i++)
	{
		hash = (str[i]) + (hash << 6) + (hash << 16) - hash;
	}
}