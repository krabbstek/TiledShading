#pragma once

#include "ConstExpr.h"

#include "graphics/Light.h"
#include "graphics/RenderTechnique.h"

extern Lights<g_LightGridSize> g_Lights;
extern vec3 g_GlobalLightPosition;
extern vec3 g_LightColors[g_LightGridSize][g_LightGridSize];

extern float g_GlobalLightIntensity;
extern vec3 g_GlobalLightPosition;

extern float g_LightIntensityMultiplier;
extern float g_LightFalloffThreshold;

extern vec3 g_LightGridOffset;
extern vec2 g_LightGridScale;
extern bool g_DynamicLights;

extern float g_Time;