#include "Random.hpp"

static uint32 Seed;

void VSGE::SetRandomSeed(uint32 _Seed) {
	Seed = _Seed;
}

int32 VSGE::Rand() {
	Seed = Seed * 214013 + 2531011;
	return (Seed >> 16) & 32767u; //32767;
}

std::string VSGE::GetRandomString(int len) {
	std::string result;
	for(int i = 0; i < len; i ++)
		result.push_back(Random(32, 70));
	return result;
}