#pragma once

#include "math/vec3.h"

struct Plane
{
	// ax + by + cz + d = 0
	float a, b, c, d;

public:
	Plane();
	Plane(float a, float b, float c, float d);
	Plane(const vec3& normal, float d);

	float Distance(const vec3& point);
};