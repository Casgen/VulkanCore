#pragma once

#include "Vec3f.h"

struct Sphere {
	Vec3f center;
	float r;

	inline bool isPointInside(const Vec3f& point) {
		return (point - center).Magnitude() <= r;
	}
};
