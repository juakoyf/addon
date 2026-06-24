#pragma once
#include <cmath>
#include <limits>

class Vector {
public:
	// data member variables
	float x, y, z;

public:
	// ctors.
	__forceinline Vector() : x{}, y{}, z{} {}
	__forceinline Vector(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}

	// at-accesors.
	__forceinline float& at(const size_t index) {
		return ((float*)this)[index];
	}
	__forceinline float& at(const size_t index) const {
		return ((float*)this)[index];
	}

	// index operators.
	__forceinline float& operator( )(const size_t index) {
		return at(index);
	}
	__forceinline const float& operator( )(const size_t index) const {
		return at(index);
	}
	__forceinline float& operator[ ](const size_t index) {
		return at(index);
	}
	__forceinline const float& operator[ ](const size_t index) const {
		return at(index);
	}

	// equality operators.
	__forceinline bool operator==(const Vector& v) const {
		return v.x == x && v.y == y && v.z == z;
	}
	__forceinline bool operator!=(const Vector& v) const {
		return v.x != x || v.y != y || v.z != z;
	}

	// copy assignment.
	__forceinline Vector& operator=(const Vector& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	// negation-operator.
	__forceinline Vector operator-() const {
		return Vector{ -x, -y, -z };
	}

	// arithmetic operators.
	__forceinline Vector operator+(const Vector& v) const {
		return {
			x + v.x,
			y + v.y,
			z + v.z
		};
	}

	__forceinline Vector operator-(const Vector& v) const {
		return {
			x - v.x,
			y - v.y,
			z - v.z
		};
	}

	__forceinline Vector operator*(const Vector& v) const {
		return {
			x * v.x,
			y * v.y,
			z * v.z
		};
	}

	__forceinline Vector operator/(const Vector& v) const {
		return {
			x / v.x,
			y / v.y,
			z / v.z
		};
	}

	// compound assignment operators.
	__forceinline Vector& operator+=(const Vector& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	__forceinline Vector& operator-=(const Vector& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	__forceinline Vector& operator*=(const Vector& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	__forceinline Vector& operator/=(const Vector& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// arithmetic operators w/ float.
	__forceinline Vector operator+(float f) const {
		return {
			x + f,
			y + f,
			z + f
		};
	}

	__forceinline Vector operator-(float f) const {
		return {
			x - f,
			y - f,
			z - f
		};
	}

	__forceinline Vector operator*(float f) const {
		return {
			x * f,
			y * f,
			z * f
		};
	}

	__forceinline Vector operator/(float f) const {
		return {
			x / f,
			y / f,
			z / f
		};
	}

	// compound assignment operators w/ float.
	__forceinline Vector& operator+=(float f) {
		x += f;
		y += f;
		z += f;
		return *this;
	}

	__forceinline Vector& operator-=(float f) {
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	__forceinline Vector& operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	__forceinline Vector& operator/=(float f) {
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	// methods.
	__forceinline void Zero() {
		x = y = z = 0.f;
	}

	__forceinline void clear() {
		x = y = z = 0.f;
	}

	__forceinline float LengthSqr() const {
		return ((x * x) + (y * y) + (z * z));
	}

	__forceinline float length_sqr() const {
		return ((x * x) + (y * y) + (z * z));
	}

	__forceinline float length_2d_sqr() const {
		return ((x * x) + (y * y));
	}

	__forceinline float Length() const {
		return sqrt(LengthSqr());
	}

	__forceinline float length() const {
		return sqrt(length_sqr());
	}

	__forceinline float Length2D() const {
		return sqrt(length_2d_sqr());
	}

	__forceinline float length_2d() const {
		return sqrt(length_2d_sqr());
	}

	__forceinline float Dot(const Vector& v) const {
		return (x * v.x + y * v.y + z * v.z);
	}

	__forceinline float dot(const Vector& v) const {
		return (x * v.x + y * v.y + z * v.z);
	}

	__forceinline float dot(float* v) const {
		return (x * v[0] + y * v[1] + z * v[2]);
	}

	__forceinline Vector cross(const Vector& v) const {
		return {
			(y * v.z) - (z * v.y),
			(z * v.x) - (x * v.z),
			(x * v.y) - (y * v.x)
		};
	}

	__forceinline float Distance(const Vector& vOther) const {
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}

	__forceinline float dist_to(const Vector& vOther) const {
		return Distance(vOther);
	}

	__forceinline float dist_to_sqr(const Vector& vOther) const {
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	__forceinline float normalize() {
		float len = length();

		(*this) /= (length() + std::numeric_limits< float >::epsilon());

		return len;
	}

	__forceinline Vector normalized() const {
		auto vec = *this;

		vec.normalize();

		return vec;
	}

	__forceinline bool IsZero() const {
		return (x == 0.f && y == 0.f && z == 0.f);
	}
};

__forceinline Vector operator*(float f, const Vector& v) {
	return v * f;
}
