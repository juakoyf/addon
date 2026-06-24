#pragma once
#include <algorithm>
#include <cmath>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

class Color {
public:
	unsigned char r, g, b, a;

	Color() : r(0), g(0), b(0), a(255) {}
	Color(int r, int g, int b, int a = 255)
		: r(r), g(g), b(b), a(a) {
	}

	inline int rInt() const { return r; }
	inline int gInt() const { return g; }
	inline int bInt() const { return b; }
	inline int aInt() const { return a; }

	inline void Set(int _r, int _g, int _b, int _a = 255) {
		r = _r; g = _g; b = _b; a = _a;
	}

	inline float rFloat() const { return r / 255.f; }
	inline float gFloat() const { return g / 255.f; }
	inline float bFloat() const { return b / 255.f; }
	inline float aFloat() const { return a / 255.f; }

	inline unsigned int ToRGBA() const {
		return (a << 24) | (r << 16) | (g << 8) | b;
	}
};
