#pragma once

#include "math/math_util.h"

constexpr unsigned int g_WindowWidth = 1280;
constexpr unsigned int g_WindowHeight = 720;

constexpr unsigned int g_TileSize = 10;

constexpr int g_NumTileCols = g_WindowWidth / g_TileSize;
constexpr int g_NumTileRows = g_WindowHeight / g_TileSize;
constexpr int g_MaxNumLightsPerTile = 1024;

constexpr int g_CubeGridSize = 6;
constexpr int g_LightGridSize = 50;

constexpr float g_FOV = DegToRad(75.0f);
constexpr float g_AspectRatio = float(g_WindowWidth) / float(g_WindowHeight);
constexpr float g_NearPlaneDepth = 1.0f;
constexpr float g_FarPlaneDepth = 100.0f;

constexpr int g_NumGraphSamples = 240;
constexpr float g_LargeGraphSizeX = 320.0f;
constexpr float g_LargeGraphSizeY = 80.0f;
constexpr float g_SmallGraphSizeX = 320.0f;
constexpr float g_SmallGraphSizeY = 40.0f;

constexpr unsigned int BIT(unsigned int bit) { return 1 << bit; }