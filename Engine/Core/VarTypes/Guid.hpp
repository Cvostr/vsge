#pragma once

#include "Base.hpp"
#include <string>

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
		/// <summary>
		/// Compare two guids
		/// </summary>
		/// <param name="g"></param>
		/// <returns></returns>
		bool operator==(const Guid& g);

		std::string ToBinaryView();

		void FromBinaryView(const std::string& bytes);

		/// <summary>
		/// Generate random guid
		/// </summary>
		Guid();
		Guid(uint32 a, uint32 b, uint32 c, uint32 d) :
			a(a), b(b), c(c), d(d) {}
	};
}