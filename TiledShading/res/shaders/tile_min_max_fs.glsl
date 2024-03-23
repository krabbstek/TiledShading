#version 450 core

layout (location = 0) out float out_MinDepth;
layout (location = 1) out float out_MaxDepth;

layout (binding = 0) uniform sampler2D u_ViewSpacePositionTexture;

uniform int u_TileSize = 1;

void main()
{
	ivec2 tileCoordinate = ivec2(gl_FragCoord.xy);
	ivec2 fullSizeCoordinate = tileCoordinate * u_TileSize;

	float minDepth = 1e10;
	float maxDepth = -1e10;
	float depth;
	ivec2 offset;
	for (int i = 0; i < u_TileSize; i++)
	{
		offset.x = i;
		for (int j = 0; j < u_TileSize; j++)
		{
			offset.y = j;
			depth = texelFetch(u_ViewSpacePositionTexture, fullSizeCoordinate + offset, 0).z;
			if (depth == 0.0)
				continue;
			minDepth = min(minDepth, depth);
			maxDepth = max(maxDepth, depth);
		}
	}

	out_MinDepth = minDepth;
	out_MaxDepth = maxDepth;
}