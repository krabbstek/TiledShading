#pragma once

#include "math/math_util.h"

constexpr unsigned int g_WindowWidth = 1280;
constexpr unsigned int g_WindowHeight = 720;
constexpr unsigned int g_TileSize = 40;
constexpr int tileCols = g_WindowWidth / g_TileSize;
constexpr int tileRows = g_WindowHeight / g_TileSize;

constexpr int g_CubeGridSize = 9;
constexpr int g_LightGridSize = 3;

constexpr float g_FOV = DegToRad(75.0f);