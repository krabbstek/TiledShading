#include "Plane.h"

Plane::Plane(float a, float b, float c, float d)
	: Plane(vec3(a, b, c), d)
{
}

Plane::Plane(const vec3& normal, float d)
{
	float length = normal.Magnitude();
	float invLength = 1.0f / length;
	a = normal.x * invLength;
	b = normal.y * invLength;
	c = normal.z * invLength;
	this->d = d * invLength;
}


float Plane::Distance(const vec3& point)
{
	return a * point.x + b * point.y + c * point.z + d;
}