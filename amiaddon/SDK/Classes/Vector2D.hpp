#pragma once
#include <cmath>

class Vector2D {
public:
    float x, y;

    Vector2D() : x(0.f), y(0.f) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    inline float Length() const {
        return std::sqrt(x * x + y * y);
    }

    inline float LengthSqr() const {
        return x * x + y * y;
    }

    inline void Zero() {
        x = y = 0.f;
    }

    inline Vector2D operator+(const Vector2D& v) const {
        return Vector2D(x + v.x, y + v.y);
    }

    inline Vector2D operator-(const Vector2D& v) const {
        return Vector2D(x - v.x, y - v.y);
    }

    inline Vector2D operator*(float f) const {
        return Vector2D(x * f, y * f);
    }

    inline Vector2D operator/(float f) const {
        return Vector2D(x / f, y / f);
    }

    inline Vector2D& operator+=(const Vector2D& v) {
        x += v.x; y += v.y;
        return *this;
    }

    inline Vector2D& operator-=(const Vector2D& v) {
        x -= v.x; y -= v.y;
        return *this;
    }

    inline Vector2D& operator/=(float f) {
        x /= f; y /= f;
        return *this;
    }

    inline Vector2D operator+(float f) const {
        return Vector2D(x + f, y + f);
    }

    inline Vector2D operator-(float f) const {
        return Vector2D(x - f, y - f);
    }

    inline Vector2D& operator+=(float f) {
        x += f; y += f;
        return *this;
    }

    inline Vector2D& operator-=(float f) {
        x -= f; y -= f;
        return *this;
    }



    inline bool IsZero() const {
        return (x == 0.f && y == 0.f);
    }
};
