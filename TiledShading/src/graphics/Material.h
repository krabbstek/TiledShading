#pragma once

#include "math/math.h"
#include "graphics/opengl/GLShader.h"

struct Material
{
public:
	vec4 albedo;
	float reflectivity;
	float shininess;
	float metalness;
	float fresnel;

	void Bind(GLShader& shader) const;
};