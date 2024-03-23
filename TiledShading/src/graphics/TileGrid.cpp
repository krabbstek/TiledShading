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


void TileGrid::ComputeLightTiles(Light* lights, int numLights, float* tileMinDepth, float* tileMaxDepth, GLShaderStorageBuffer& lightIndexSSBO, GLShaderStorageBuffer& tileIndexSSBO)
{
	float lightDistanceThreshold = -sqrt(g_LightIntensityMultiplier / g_LightFalloffThreshold);
	float r2 = lightDistanceThreshold * lightDistanceThreshold;
	float planeDistance;

	float tanY = tan(0.5f * g_FOV);
	float halfTanPerTile = tanY * (1.0f / float(g_NumTileRows));

	for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
		for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
			m_LightTiles[tileRow][tileCol].clear();

	for (int lightIndex = 0; lightIndex < numLights; lightIndex++)
	{
		Light& light = lights[lightIndex];

#if 0
		// Check if the origin is within the threshold radius. If it is: check all tiles
		if (light.viewSpacePosition.Dot(light.viewSpacePosition) <= r2)
		{
			for (int tileRow = 0; tileRow < g_NumTileRows; tileRow++)
			{
				for (int tileCol = 0; tileCol < g_NumTileCols; tileCol++)
				{
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
		else
		{
			int leftLim, rightLim, bottomLim, topLim;

			vec2 d = vec2(light.viewSpacePosition.x, light.viewSpacePosition.z);
			float d2 = d.Dot(d);
			// Compute left and right tile limits
			float tNorm = sqrt(d2 - r2);
			float dInv = 1.0f / sqrt(d2);
			float cos = tNorm * dInv;
			float sin = lightDistanceThreshold * dInv;

			vec2 t = vec2(cos * d.x + sin * d.y, cos * d.y - sin * d.x);
			if (t.y >= 0.0f)
				leftLim = 0;
			else
			{
				float tan = t.x / t.y;
				float normalizedTan = tan / (0.5f * halfTanPerTile);
				leftLim = int(floorf(normalizedTan + 0.5f * float(g_NumTileRows)));
			}

			t = vec2(cos * d.x - sin * d.y, cos * d.y + sin * d.x);
			if (t.y >= 0.0f)
				rightLim = g_NumTileCols;
			else
			{
				float tan = t.x / t.y;
				float normalizedTan = tan / (0.5f * halfTanPerTile);
				rightLim = int(ceilf(normalizedTan + 0.5f * float(g_NumTileRows)));
			}

			d = vec2(light.viewSpacePosition.y, light.viewSpacePosition.z);
			d2 = d.Dot(d);
			// Compute bottom and top tile limits
			tNorm = sqrt(d2 - r2);
			dInv = 1.0f / sqrt(d2);
			cos = tNorm * dInv;
			sin = lightDistanceThreshold * dInv;

			t = vec2(cos * d.x + sin * d.y, cos * d.y - sin * d.x);
			if (t.y >= 0.0f)
				bottomLim = 0;
			else
			{
				float tan = t.x / t.y;
				float normalizedTan = tan / (0.5f * halfTanPerTile);
				bottomLim = int(floorf(normalizedTan + 0.5f * float(g_NumTileRows)));
			}

			t = vec2(cos * d.x - sin * d.y, cos * d.y + sin * d.x);
			if (t.y >= 0.0f)
				topLim = g_NumTileCols;
			else
			{
				float tan = t.x / t.y;
				float normalizedTan = tan / (0.5f * halfTanPerTile);
				topLim = int(ceilf(normalizedTan + 0.5f * float(g_NumTileRows)));
			}

			leftLim = std::max(leftLim, 0);
			rightLim = std::min(rightLim, g_NumTileCols);
			bottomLim = std::max(bottomLim, 0);
			topLim = std::min(topLim, g_NumTileRows);

			for (int row = bottomLim; row < topLim; row++)
				for (int col = leftLim; col < rightLim; col++)
					m_LightTiles[row][col].push_back(lightIndex);
		}
#elif 0

		planeDistance = bottomPlanes[0].Distance(light.viewSpacePosition);
		if (planeDistance <= lightDistanceThreshold)
			continue;

		planeDistance = topPlanes[g_NumTileRows - 1].Distance(light.viewSpacePosition);
		if (planeDistance <= lightDistanceThreshold)
			continue;

		planeDistance = leftPlanes[0].Distance(light.viewSpacePosition);
		if (planeDistance <= lightDistanceThreshold)
			continue;

		planeDistance = rightPlanes[g_NumTileRows - 1].Distance(light.viewSpacePosition);
		if (planeDistance <= lightDistanceThreshold)
			continue;



#else
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
				planeDistance = leftPlanes[tileCol].Distance(light.viewSpacePosition);
				if (planeDistance <= lightDistanceThreshold)
					continue;

				planeDistance = rightPlanes[tileCol].Distance(light.viewSpacePosition);
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
#endif
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