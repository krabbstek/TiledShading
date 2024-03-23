#pragma once

#include "math/math.h"

struct Camera
{
public:
	vec3 position;
	// Y-X-Z
	vec3 rotation;
	mat4 projectionMatrix;
	
	mat4 GetViewMatrix();
};