#pragma once

#include "Base.hpp"
#include <string>

class Guid {
	public:
		union {
			uint32 Values[4];
			uint8 RawValues[16];

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

		bool operator!=(const Guid& g);

		std::string toBinaryView();

		void fromBinaryView(const std::string& bytes);

		std::string toString() const;

		/// <summary>
		/// Generate random guid
		/// </summary>
		Guid();
		Guid(uint32_t a, uint32_t b, uint32_t c, uint32_t d) :
			a(a), b(b), c(c), d(d) {}
	};