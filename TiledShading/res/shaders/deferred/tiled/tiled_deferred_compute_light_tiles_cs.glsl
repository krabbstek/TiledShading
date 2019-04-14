#version 430 core

#define TILE_SIZE 20

layout (local_size_x = TILE_SIZE, local_size_y = TILE_SIZE) in;

uniform int u_MaxNumLightsPerTile = 256;
uniform float u_FarPlaneDepth;

shared int tileLightCount;
shared int minIDepth;
shared int maxIDepth;
shared float minDepth;
shared float maxDepth;

layout (binding = 0) uniform sampler2D u_ViewSpacePositionTexture;

struct Light
{
	vec4 viewSpacePosition;
	vec4 color;
};

struct Plane
{
	float a;
	float b;
	float c;
	float d;
};

/// Lights, containing view space position and color
layout (std430, binding = 3) readonly buffer LightBuffer
{
	Light lights[];
};

/// Integer array containing pure indices to above lights
layout (std430, binding = 4) writeonly buffer LightIndexBuffer
{
	int lightIndices[];
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


int TileIndex(int tileCol, int tileRow)
{
	return tileCol + tileRow * int(gl_NumWorkGroups.x);
}

float PlaneDistance(Plane plane, vec4 point)
{
	return plane.a * point.x + plane.b * point.y + plane.c * point.z + plane.d;
}

void main()
{
	int tileCol = int(gl_WorkGroupID.x);
	int tileRow = int(gl_WorkGroupID.y);
	int threadsPerTile = int(gl_WorkGroupSize.x * gl_WorkGroupSize.y);
	int tileIndex = TileIndex(tileCol, tileRow);
	int lightIndicesOffset = tileIndex * u_MaxNumLightsPerTile;

	Plane leftPlane = leftPlanes[tileCol];
	Plane rightPlane = rightPlanes[tileCol];
	Plane bottomPlane = bottomPlanes[tileRow];
	Plane topPlane = topPlanes[tileRow];
	float planeDistance;
	float threshold;

	if (gl_LocalInvocationIndex == 0)
	{
		tileLightCount = 0;
		minIDepth = 0x7FFFFFFF;
		maxIDepth = -0x7FFFFFFF;
	}

	barrier();

	ivec2 texCoords = ivec2(gl_WorkGroupID.xy * gl_WorkGroupSize.xy + gl_LocalInvocationID.xy);
	float depth = texelFetch(u_ViewSpacePositionTexture, texCoords, 0).z;
	int iDepth = int(depth * float(0x7FFFFFFF) / u_FarPlaneDepth);

	atomicMin(minIDepth, iDepth);
	atomicMax(maxIDepth, iDepth);

	barrier();

	if (gl_LocalInvocationIndex == 0)
	{
		minDepth = float(minIDepth) * u_FarPlaneDepth / float(0x7FFFFFFF);
		maxDepth = float(maxIDepth) * u_FarPlaneDepth / float(0x7FFFFFFF);
	}

	barrier();

	int id;
	int numLights = lights.length();
	Light light;
	for (int i = int(gl_LocalInvocationIndex); i < numLights; i += threadsPerTile)
	{
		light = lights[i];
		threshold = -1.0 / sqrt(light.color.a);
		
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

		planeDistance = light.viewSpacePosition.z - maxDepth;
		if (planeDistance >= -threshold)
			continue;

		planeDistance = light.viewSpacePosition.z - minDepth;
		if (planeDistance <= threshold)
			continue;
			
		id = min(atomicAdd(tileLightCount, 1), u_MaxNumLightsPerTile - 1);
		lightIndices[lightIndicesOffset + id] = i;
	}

	barrier();

	if (gl_LocalInvocationIndex == 0)
	{	
		if (tileLightCount < u_MaxNumLightsPerTile - 1)
			lightIndices[lightIndicesOffset + tileLightCount] = -1;
	}
}