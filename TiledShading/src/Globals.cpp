#include "graphics/Light.h"

#include "ConstExpr.h"

Light g_LightGrid[g_LightGridSize][g_LightGridSize];
vec3 g_LightColors[g_LightGridSize][g_LightGridSize];

float g_LightIntensityMultiplier = 0.1f;
float g_LightFalloffThreshold = 0.01f;