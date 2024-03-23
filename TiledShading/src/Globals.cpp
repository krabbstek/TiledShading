#include "graphics/Light.h"

#include "ConstExpr.h"

Light g_LightGrid[g_LightGridSize][g_LightGridSize];

int g_PlotOffset = 0;

float g_LightIntensityMultiplier = 0.1f;
float g_LightFalloffThreshold = 0.01f;