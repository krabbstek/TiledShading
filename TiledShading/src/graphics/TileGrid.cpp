#include "TileGrid.h"

#include "Globals.h"

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

		m_LeftPlanes[col].a = normal.x;
		m_LeftPlanes[col].c = normal.z;
	}

	// Right planes
	for (int col = 0, numHalfTanPerTile = -(g_NumTileCols - 2); col < g_NumTileCols; col++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = -1.0f;
		normal.y = 0.0f;
		normal.z = -tan;
		normal.Normalize();

		m_RightPlanes[col].a = normal.x;
		m_RightPlanes[col].c = normal.z;
	}

	// Bottom planes
	for (int row = 0, numHalfTanPerTile = -g_NumTileRows; row < g_NumTileRows; row++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = 0.0f;
		normal.y = 1.0f;
		normal.z = tan;
		normal.Normalize();

		m_BottomPlanes[row].b = normal.y;
		m_BottomPlanes[row].c = normal.z;
	}

	// Top planes
	for (int row = 0, numHalfTanPerTile = -(g_NumTileRows - 2); row < g_NumTileCols; row++, numHalfTanPerTile += 2)
	{
		float tan = halfTanPerTile * float(numHalfTanPerTile);
		normal.x = 0.0f;
		normal.y = -1.0f;
		normal.z = -tan;
		normal.Normalize();

		m_TopPlanes[row].b = normal.y;
		m_TopPlanes[row].c = normal.z;
	}

	return;

	constexpr float fovPerTile = g_FOV / float(g_NumTileRows);
	constexpr float halfFovPerTile = 0.5f * fovPerTile;

	constexpr int rightLim = g_NumTileCols - 2;
	constexpr int leftLim = -rightLim;
	constexpr int topLim = g_NumTileRows - 2;
	constexpr int bottomLim = -topLim;

	int lowIndex = 0;
	int highIndex = 1;

	float c, s;
	normal;

	for (int col = leftLim; col <= rightLim; col += 2)
	{
		c = cos(col * halfFovPerTile);
		s = sin(col * halfFovPerTile);

		normal.x = c;
		normal.z = s;

		m_LeftPlanes[highIndex++] = Plane(normal, 0.0f);
		m_RightPlanes[lowIndex++] = Plane(-normal, 0.0f);
	}

	m_LeftPlanes[0] = Plane(cos(halfFovPerTile * g_NumTileCols), 0.0f, sin(halfFovPerTile * -g_NumTileCols), 0.0f);
	m_RightPlanes[g_NumTileCols - 1] = Plane(-cos(halfFovPerTile * g_NumTileCols), 0.0f, -sin(halfFovPerTile * g_NumTileCols), 0.0f);

	normal.x = 0.0f;
	lowIndex = 0;
	highIndex = 1;

	for (int row = bottomLim; row <= topLim; row += 2)
	{
		c = cos(row * halfFovPerTile);
		s = sin(row * halfFovPerTile);

		normal.y = c;
		normal.z = s;

		m_BottomPlanes[highIndex++] = Plane(normal, 0.0f);
		m_TopPlanes[lowIndex++] = Plane(-normal, 0.0f);
	}

	m_BottomPlanes[0] = Plane(0.0f, cos(halfFovPerTile * g_NumTileCols), sin(halfFovPerTile * -g_NumTileCols), 0.0f);
	m_TopPlanes[g_NumTileRows] = Plane(0.0f, -cos(halfFovPerTile * g_NumTileCols), -sin(halfFovPerTile * g_NumTileCols), 0.0f);
}


void TileGrid::ComputeLightTiles(Light* lights, int numLights, float* tileMinDepth, float* tileMaxDepth, GLShaderStorageBuffer& lightIndexSSBO, GLShaderStorageBuffer& tileIndexSSBO)
{
	float lightDistanceThreshold = -sqrt(g_LightIntensityMultiplier / g_LightFalloffThreshold);
	float planeDistance;

	for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
		for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
			m_LightTiles[tileRow][tileCol].clear();

	for (int lightIndex = 0; lightIndex < numLights; lightIndex++)
	{
		Light& light = lights[lightIndex];

		for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
		{
			planeDistance = m_BottomPlanes[tileRow].Distance(light.viewSpacePosition);
			if (planeDistance <= lightDistanceThreshold)
				continue;

			planeDistance = m_TopPlanes[tileRow].Distance(light.viewSpacePosition);
			if (planeDistance <= lightDistanceThreshold)
				continue;

			for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
			{
				planeDistance = m_LeftPlanes[tileCol].Distance(light.viewSpacePosition);
				if (planeDistance <= lightDistanceThreshold)
					continue;

				planeDistance = m_RightPlanes[tileCol].Distance(light.viewSpacePosition);
				if (planeDistance <= lightDistanceThreshold)
					continue;

				planeDistance = light.viewSpacePosition.z - tileMinDepth[tileRow * g_NumTileCols + tileCol];
				if (planeDistance <= lightDistanceThreshold)
					continue;

				planeDistance = tileMaxDepth[tileRow * g_NumTileCols + tileCol] - light.viewSpacePosition.z;
				if (planeDistance <= lightDistanceThreshold)
					continue;

				m_LightTiles[tileRow][tileCol].push_back(lightIndex);
			}
		}
	}

	int memoryLayoutIndex = 0;
	m_LightIndices.clear();

	for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
	{
		for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
		{
			m_TileIndexMemoryLayout[memoryLayoutIndex++] = int(m_LightIndices.size());
			numLights = int(m_LightTiles[tileRow][tileCol].size());
			m_TileIndexMemoryLayout[memoryLayoutIndex++] = numLights;

			for (int i = 0; i < numLights; i++)
				m_LightIndices.emplace_back(m_LightTiles[tileRow][tileCol][i]);
		}
	}

	if (!m_LightIndices.size())
		m_LightIndices.push_back(0);

	lightIndexSSBO.SetData(m_LightIndices.data(), unsigned int(m_LightIndices.size() * sizeof(int)));
	tileIndexSSBO.SetData(m_TileIndexMemoryLayout, sizeof(m_TileIndexMemoryLayout));
}