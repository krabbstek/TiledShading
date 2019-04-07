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

private:
	// Memory layout
	// [ 3, 4, 5 ]
	// [ 0, 1, 2 ]
	Plane m_LeftPlanes[g_NumTileCols];
	Plane m_RightPlanes[g_NumTileCols];
	Plane m_BottomPlanes[g_NumTileRows];
	Plane m_TopPlanes[g_NumTileRows];
	int m_TileIndexMemoryLayout[2 * g_NumTileRows * g_NumTileCols];
	std::vector<int> m_LightTiles[g_NumTileRows][g_NumTileCols];
	std::vector<int> m_LightIndices;
};