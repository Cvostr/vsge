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

		bool operator==(const Guid& g) const;

		bool operator!=(const Guid& g) const;

		std::string toBinaryView() const;

		void fromBinaryView(const std::string& bytes);

		std::string toString() const;

		bool isValid() const;

		Guid();
		Guid(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
		Guid(uint8_t values[16]);
		Guid(const std::string& str);
	};