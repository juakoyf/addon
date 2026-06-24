#pragma once
#include <cmath>
#include <limits>

class QAngle {
public:
    float x; // pitch
    float y; // yaw
    float z; // roll

    QAngle() : x(0.f), y(0.f), z(0.f) {}
    QAngle(float x, float y, float z = 0.f) : x(x), y(y), z(z) {}

    inline void Zero() {
        x = y = z = 0.f;
    }

    inline float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    inline float LengthSqr() const {
        return x * x + y * y + z * z;
    }

    inline bool IsZero() const {
        return (x == 0.f && y == 0.f && z == 0.f);
    }

    inline QAngle operator+(const QAngle& v) const {
        return QAngle(x + v.x, y + v.y, z + v.z);
    }

    inline QAngle operator-(const QAngle& v) const {
        return QAngle(x - v.x, y - v.y, z - v.z);
    }

    inline QAngle operator*(float f) const {
        return QAngle(x * f, y * f, z * f);
    }

    inline QAngle& operator+=(const QAngle& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline QAngle& operator-=(const QAngle& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
};

inline void NormalizeAngle(float& angle) {
	float rot;

	// bad number.
	if (!std::isfinite(angle)) {
		angle = 0.f;
		return;
	}

	// no need to normalize this angle.
	if (angle >= -180.f && angle <= 180.f)
		return;

	// get amount of rotations needed.
	rot = std::round(std::abs(angle / 360.f));

	// normalize.
	angle = (angle < 0.f) ? angle + (360.f * rot) : angle - (360.f * rot);
}

inline float NormalizedAngle(float angle) {
	NormalizeAngle(angle);
	return angle;
}
