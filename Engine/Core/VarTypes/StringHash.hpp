#pragma once

#include <string>
#include "Base.hpp"

namespace VSGE {
	class StringHash {
	private:
		uint32 hash;
	public:

		StringHash() :
			hash(0)
		{}

		void Hash(const std::string& str);
		uint32 GetHash() { return hash; }
	};
}