#pragma once

#include "math/vec4.h"

struct Light
{
	vec4 viewSpacePosition;
	vec4 color;
};

template <int lightGridSize>
struct Lights
{
	Light globalLight;
	Light lightGrid[lightGridSize][lightGridSize];
};