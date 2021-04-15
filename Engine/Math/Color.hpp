#pragma once

namespace VSGE {
	class Color {
	public:
		/// <summary>
		/// r, g, b, a channels values
		/// </summary>
		float r;
		float g;
		float b;
		float a;

		Color() : r(1.f), g(1.f), b(1.f), a(1.f) {}
		Color(float rgb) : r(rgb), g(rgb), b(rgb), a(1.f) {}
		Color(float r, float g, float b, float a = 1) : r(r), g(g), b(b), a(a) {}

		bool operator==(const Color& c) const {
			return r == c.r && g == c.g && b == c.b && a == c.a;
		}

		bool operator!=(const Color& c) const {
			return r != c.r && g != c.g && b != c.b && a != c.a;
		}

		Color operator+(const Color& c) const {
			return Color(r + c.r, g + c.g, b + c.b, a + c.a);
		}
	};
}