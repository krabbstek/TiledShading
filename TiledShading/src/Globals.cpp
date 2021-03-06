#include "graphics/Light.h"

#include "ConstExpr.h"

Lights<g_LightGridSize> g_Lights;
vec3 g_LightColors[g_LightGridSize][g_LightGridSize];

float g_GlobalLightIntensity= 100.0f;
vec3 g_GlobalLightPosition = vec3(0.0f, 5.0f, 0.0f);

float g_LightIntensityMultiplier = 0.3f;
float g_LightFalloffThreshold = 0.01f;

vec3 g_LightGridOffset = vec3(0.0f, 0.6f, 0.0f);
//vec3 g_LightGridOffset = vec3(0.0f, -0.4f, 0.0f);
vec2 g_LightGridScale = vec2(0.7f); //vec2(1.3f);
bool g_DynamicLights = true;

float g_HeatmapAlpha = 0.0f;

float g_Time = 0.0f;