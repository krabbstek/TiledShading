#pragma once

#include "math/math.h"

struct Material
{
public:
	vec4 albedo;
	float reflectivity;
	float shininess;
	float metalness;
	float fresnel;
};