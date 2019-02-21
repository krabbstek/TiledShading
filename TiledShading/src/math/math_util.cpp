#include "math_util.h"

constexpr float Clamp(float val, float min, float max)
{
	if (val <= min)
		return min;
	else if (val >= max)
		return max;
	return val;
}


float CubicInterpolate(float t)
{
	float t2 = t * t;
	return 3 * t2 - 2 * t * t2;
}