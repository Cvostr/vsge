#pragma once

#include "VarTypes/Base.hpp"
#include <string>

namespace VSGE {
	void SetRandomSeed(uint32 _Seed);
	int32 Rand();

	template<typename T>
	T Random(T min, T max) {
		return static_cast<T>(min + Rand() % (int)(max - min));
	}

	std::string GetRandomString(int len);

}