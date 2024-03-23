#pragma once

#include "math/math_util.h"

constexpr unsigned int g_WindowWidth = 1200;
constexpr unsigned int g_WindowHeight = 900;
constexpr unsigned int g_TileSize = 300;
constexpr int g_NumTileCols = g_WindowWidth / g_TileSize;
constexpr int g_NumTileRows = g_WindowHeight / g_TileSize;

constexpr int g_CubeGridSize = 9;
constexpr int g_LightGridSize = 15;

constexpr float g_FOV = DegToRad(75.0f);
constexpr float g_AspectRatio = float(g_WindowWidth) / float(g_WindowHeight);
constexpr float g_NearPlaneDepth = 0.1f;
constexpr float g_FarPlaneDepth = 100.0f;

constexpr int g_NumGraphSamples = 240;
constexpr float g_GraphSizeX = 320.0f;
constexpr float g_GraphSizeY = 80.0f;