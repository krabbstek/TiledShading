#include "TileGrid.h"

#include "Globals.h"

#include <algorithm>

TileGrid::TileGrid()
{
	// Make sure memory is reset
	memset(m_TileIndexMemoryLayout, 0, sizeof(m_TileIndexMemoryLayout));

	// Compute tile planes
	float tanY = tanf(0.5f * g_FOV);
	float tanX = g_AspectRatio * tanY;
	float halfTanPerTile = tanY * (1.0f / g_NumTileRows);

	vec3 normal;

	// Left planes
	for (int col = 0, numHalfTanPerTile = -g_NumTileCols; col < g_NumTileCols; col++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = 1.0f;
		normal.y = 0.0f;
		normal.z = tan;
		normal.Normalize();

		leftPlanes[col].a = normal.x;
		leftPlanes[col].c = normal.z;
	}

	// Right planes
	for (int col = 0, numHalfTanPerTile = -(g_NumTileCols - 2); col < g_NumTileCols; col++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = -1.0f;
		normal.y = 0.0f;
		normal.z = -tan;
		normal.Normalize();

		rightPlanes[col].a = normal.x;
		rightPlanes[col].c = normal.z;
	}

	// Bottom planes
	for (int row = 0, numHalfTanPerTile = -g_NumTileRows; row < g_NumTileRows; row++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = 0.0f;
		normal.y = 1.0f;
		normal.z = tan;
		normal.Normalize();

		bottomPlanes[row].b = normal.y;
		bottomPlanes[row].c = normal.z;
	}

	// Top planes
	for (int row = 0, numHalfTanPerTile = -(g_NumTileRows - 2); row < g_NumTileCols; row++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = 0.0f;
		normal.y = -1.0f;
		normal.z = -tan;
		normal.Normalize();

		topPlanes[row].b = normal.y;
		topPlanes[row].c = normal.z;
	}
}


void TileGrid::ComputeLightTiles(Light* lights, int numLights, float* tileMinDepth, float* tileMaxDepth, GLShaderStorageBuffer& lightIndexSSBO)
{
	// Minimum value returned from Plane::Distance for light to pass test
	float lightDistanceThreshold = -sqrt(g_LightIntensityMultiplier / g_LightFalloffThreshold);
	// Pre-allocation
	float planeDistance;

	// Clear the lights in each tile
	for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
		for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
			m_LightTiles[tileRow][tileCol].clear();

	// Iterate through all lights and compare to planes
	for (int lightIndex = 0; lightIndex < numLights; lightIndex++)
	{
		Light& light = lights[lightIndex];

		for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
		{
			planeDistance = bottomPlanes[tileRow].Distance(light.viewSpacePosition);
			if (planeDistance <= lightDistanceThreshold)
				continue;

			planeDistance = topPlanes[tileRow].Distance(light.viewSpacePosition);
			if (planeDistance <= lightDistanceThreshold)
				continue;

			for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
			{
				// If light is to the left of current plane, then all following planes will also fail the test => break instead of continue
				planeDistance = leftPlanes[tileCol].Distance(light.viewSpacePosition);
				if (planeDistance <= lightDistanceThreshold)
					break;

				planeDistance = rightPlanes[tileCol].Distance(light.viewSpacePosition);
				if (planeDistance <= lightDistanceThreshold)
					continue;

				planeDistance = light.viewSpacePosition.z - tileMinDepth[tileRow * g_NumTileCols + tileCol];
				if (planeDistance <= lightDistanceThreshold)
					continue;

				planeDistance = tileMaxDepth[tileRow * g_NumTileCols + tileCol] - light.viewSpacePosition.z;
				if (planeDistance <= lightDistanceThreshold)
					continue;

				// Light passed all plane tests: add to tile list
				m_LightTiles[tileRow][tileCol].push_back(lightIndex);
			}
		}
	}

	int tileMemoryIndex = 0;
	int size;

	// Copy light indices to m_LightIndices, max g_MaxNumLightsPerTile lights per tile
	for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
	{
		for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
		{
			std::vector<int>& tile = m_LightTiles[tileRow][tileCol];
			size = std::min(int(tile.size()), int(g_MaxNumLightsPerTile));
			memcpy(&m_LightIndices[tileMemoryIndex], tile.data(), size * sizeof(int));
			if (size < g_MaxNumLightsPerTile)
				m_LightIndices[tileMemoryIndex + size] = -1;

			tileMemoryIndex += g_MaxNumLightsPerTile;
		}
	}

	// Set data for light index SSBO
	lightIndexSSBO.SetData(m_LightIndices, sizeof(m_LightIndices));
}