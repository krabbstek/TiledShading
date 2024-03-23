#pragma once

#include "ConstExpr.h"

#include "graphics/Light.h"

extern Light g_LightGrid[g_LightGridSize][g_LightGridSize];

extern int g_PlotOffset;

extern float g_LightIntensityMultiplier;
extern float g_LightFalloffThreshold;