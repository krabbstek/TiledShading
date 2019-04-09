#pragma once

#include "ConstExpr.h"

#include "graphics/Light.h"
#include "graphics/RenderTechnique.h"

extern Light g_LightGrid[g_LightGridSize][g_LightGridSize];
extern vec3 g_LightColors[g_LightGridSize][g_LightGridSize];

extern float g_LightIntensityMultiplier;
extern float g_LightFalloffThreshold;