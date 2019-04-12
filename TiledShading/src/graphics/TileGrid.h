#pragma once

#include "Light.h"
#include "Plane.h"
#include "ConstExpr.h"
#include "opengl/OpenGL.h"

#include <vector>

class TileGrid
{
public:
	TileGrid();
	~TileGrid() {}

	void ComputeLightTiles(Light* lights, int numLights, float* tileMinDepth, float* tileMaxDepth, GLShaderStorageBuffer& lightIndexSSBO, GLShaderStorageBuffer& tileIndexSSBO);

public:
	// Memory layout
	// [ 3, 4, 5 ]
	// [ 0, 1, 2 ]
	Plane leftPlanes[g_NumTileCols];
	Plane rightPlanes[g_NumTileCols];
	Plane bottomPlanes[g_NumTileRows];
	Plane topPlanes[g_NumTileRows];
private:
	int m_TileIndexMemoryLayout[2 * g_NumTileRows * g_NumTileCols];
	std::vector<int> m_LightTiles[g_NumTileRows][g_NumTileCols];
	int m_LightIndices[g_NumTileRows * g_NumTileCols * g_MaxNumLightsPerTile];
};