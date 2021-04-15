#include "Guid.hpp"
#include "../Random.hpp"

using namespace VSGE;

Guid::Guid() {
	for (int i = 0; i < 16; i++) {
		RawValues[i] = Random(0, 255);
	}
}

bool Guid::operator==(const Guid& g) {
	for (int i = 0; i < 4; i++) {
		if (Values[i] != g.Values[i])
			return false;
	}
	return true;
}