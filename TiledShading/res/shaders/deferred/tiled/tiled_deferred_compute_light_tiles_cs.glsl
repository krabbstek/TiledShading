#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

uniform int u_MaxNumLightsPerTile = 1024;
uniform float u_LightFalloffThreshold;

struct Light
{
	vec4 viewSpacePosition;
	vec4 color;
};

struct TileLightIndex
{
	int offset;
	int count;
};

struct Plane
{
	float a;
	float b;
	float c;
	float d;
};

/// Lights, containing view space position and color
layout (std430, binding = 3) buffer LightBuffer
{
	Light lights[];
};

/// Integer array containing pure indices to above lights
layout (std430, binding = 4) buffer LightIndexBuffer
{
	int lightIndices[];
};

/// Contains the number of lights for each tile
layout (std430, binding = 5) buffer SizeBuffer
{
	int numLightsPerTile[];
};

layout (std430, binding = 10) buffer LeftPlaneBuffer
{
	Plane leftPlanes[];
};

layout (std430, binding = 11) buffer RightPlaneBuffer
{
	Plane rightPlanes[];
};

layout (std430, binding = 12) buffer BottomPlaneBuffer
{
	Plane bottomPlanes[];
};

layout (std430, binding = 13) buffer TopPlaneBuffer
{
	Plane topPlanes[];
};


uint TileIndex(uint tileCol, uint tileRow)
{
	return tileCol + tileRow * gl_NumWorkGroups.x;
}

float PlaneDistance(Plane plane, vec4 point)
{
	return plane.a * point.x + plane.b * point.y + plane.c * point.z + plane.d;
}

void main()
{
	int id;
	uint threadsPerTile = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
	uint tileIndex = TileIndex(gl_WorkGroupID.x, gl_WorkGroupID.y);
	int lightIndicesOffset = int(tileIndex) * u_MaxNumLightsPerTile;
	Plane leftPlane = leftPlanes[tileIndex];
	Plane rightPlane = rightPlanes[tileIndex];
	Plane bottomPlane = bottomPlanes[tileIndex];
	Plane topPlane = topPlanes[tileIndex];
	float planeDistance;
	float threshold = -u_LightFalloffThreshold;

	uint numLights = lights.length();
	Light light;
	for (uint i = gl_LocalInvocationIndex; i < numLights; i += threadsPerTile)
	{
		light = lights[i];

		planeDistance = PlaneDistance(leftPlane, light.viewSpacePosition);
		if (planeDistance <= threshold)
			continue;
		planeDistance = PlaneDistance(rightPlane, light.viewSpacePosition);
		if (planeDistance <= threshold)
			continue;
		planeDistance = PlaneDistance(bottomPlane, light.viewSpacePosition);
		if (planeDistance <= threshold)
			continue;
		planeDistance = PlaneDistance(topPlane, light.viewSpacePosition);
		if (planeDistance <= threshold)
			continue;

		id = min(atomicAdd(numLightsPerTile[tileIndex], 1), u_MaxNumLightsPerTile - 1);
		lightIndices[lightIndicesOffset + id] = int(i);
	}
}