#pragma once

#pragma warning(disable: 4244)

#define PI 3.141592653589793

inline constexpr float DegToRad(float degrees)
{
	return degrees * (PI / 180.0);
}

inline constexpr float RadToDeg(float radians)
{
	return radians * (180.0 / PI);
}


constexpr float Clamp(float val, float min, float max);


inline constexpr float LinearInterpolate(float t, float t0, float t1)
{
	return t * t0 + (1.0f - t) * t1;
}

float CubicInterpolate(float t);

#pragma warning(default: 4244)