#pragma once

#include "Base.hpp"
#include <string>

class Guid {
	public:
		union {
			uint32_t Values[4];
			uint8_t RawValues[16];

			struct {
				uint32_t a;
				uint32_t b;
				uint32_t c;
				uint32_t d;
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

		bool isValid() const;

		/// <summary>
		/// Generate random guid
		/// </summary>
		Guid();
		Guid(uint32_t a, uint32_t b, uint32_t c, uint32_t d) :
			a(a), b(b), c(c), d(d) {}
	};