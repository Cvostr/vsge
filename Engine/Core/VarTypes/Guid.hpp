#pragma once

#include "Base.hpp"

namespace VSGE {
	class Guid {
	public:
		union {
			uint32 Values[4];
			byte RawValues[16];

			struct {
				uint32 a;
				uint32 b;
				uint32 c;
				uint32 d;
			};
		};
		bool operator==(const Guid& g);
		/// <summary>
		/// Generate random guid
		/// </summary>
		Guid();
		Guid(uint32 a, uint32 b, uint32 c, uint32 d) :
			a(a), b(b), c(c), d(d) {}
	};
}