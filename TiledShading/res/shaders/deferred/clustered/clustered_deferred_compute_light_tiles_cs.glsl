#version 430 core

#define TILE_SIZE 10

#define BIT(x) (1 << x)

layout (local_size_x = TILE_SIZE, local_size_y = TILE_SIZE) in;

uniform int u_MaxNumLightsPerTile = 256;
uniform float u_FarPlaneDepth;

uniform mat4 u_ProjectionMatrix;

shared int s_LightIndicesOffset;
shared int s_TileLightCount;
shared int s_TileLightIndices[1024];

shared int s_MinIDepth;
shared int s_MaxIDepth;
shared float s_MinDepth;
shared float s_MaxDepth;

shared uint s_DepthMask;

layout (binding = 0) uniform sampler2D u_ViewSpacePositionTexture;

struct Light
{
	vec4 viewSpacePosition;
	vec4 color;
};

struct LightIndex
{
	int index;
	uint depthMask;
};

struct TileLights
{
	int offset;
	int lightCount;
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
layout (std430, binding = 4) buffer LightIndexBuffer
{
	int totalNumberOfIndices;
	LightIndex lightIndices[];
};

/// Integer array of tile indices in lightIndices
layout (std430, binding = 5) writeonly buffer TileIndexBuffer
{
	TileLights tileLights[];
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

uint DepthMaskBit(vec4 viewSpacePosition)
{
	vec4 clipSpacePosition = u_ProjectionMatrix * viewSpacePosition;
	float clipSpaceDepth = clipSpacePosition.z / clipSpacePosition.w;
	uint bit = BIT(int(16.0 * (clipSpaceDepth + 1.0)));
	return bit;
}

uint LightDepthMask(vec4 viewSpacePosition, float radius)
{
	return ~0;
}

void main()
{
	// Init group and local invocation variables
	int tileCol = int(gl_WorkGroupID.x);
	int tileRow = int(gl_WorkGroupID.y);
	int threadsPerTile = int(gl_WorkGroupSize.x * gl_WorkGroupSize.y);
	int tileIndex = TileIndex(tileCol, tileRow);
	int lightIndicesOffset = tileIndex * u_MaxNumLightsPerTile;
	ivec2 texCoords = ivec2(gl_WorkGroupID.xy * gl_WorkGroupSize.xy + gl_LocalInvocationID.xy);

	// Get planes from list of planes for current tile
	Plane leftPlane = leftPlanes[tileCol];
	Plane rightPlane = rightPlanes[tileCol];
	Plane bottomPlane = bottomPlanes[tileRow];
	Plane topPlane = topPlanes[tileRow];
	float planeDistance;
	float radius;
	float threshold;

	// Only initialize shared variables from one thread
	if (gl_LocalInvocationIndex == 0)
	{
		s_TileLightCount = 0;
		s_MinIDepth = 0x7FFFFFFF;
		s_MaxIDepth = -0x7FFFFFFF;
		s_DepthMask = 0;
	}

	barrier();

	// Compute depth mask
	atomicOr(s_DepthMask, DepthMaskBit(texelFetch(u_ViewSpacePositionTexture, texCoords, 0)));

	barrier();

	int id;
	int numLights = lights.length();
	Light light;
	for (int i = int(gl_LocalInvocationIndex); i < numLights; i += threadsPerTile)
	{
		light = lights[i];
		radius = 1.0 / sqrt(light.color.a);
		threshold = -radius;
		
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


			
		// If light passed all planes and depth tests: add to list of lights
		id = min(atomicAdd(s_TileLightCount, 1), u_MaxNumLightsPerTile - 1);
		s_TileLightIndices[id] = i;
	}

	barrier();

	// Atomically allocate enough space in lightIndices to put tile's light indices into
	numLights = min(s_TileLightCount, u_MaxNumLightsPerTile);
	if (gl_LocalInvocationIndex == 0)
	{
		s_LightIndicesOffset = atomicAdd(totalNumberOfIndices, numLights);
	}

	barrier();

	// Copy light indices from tileLightIndices to lightIndices
	for (int i = int(gl_LocalInvocationIndex); i < numLights; i += threadsPerTile)
		lightIndices[s_LightIndicesOffset + i].index = s_TileLightIndices[i];

	if (gl_LocalInvocationIndex == 0)
	{	
		TileLights currentTile;
		currentTile.offset = s_LightIndicesOffset;
		currentTile.lightCount = numLights;
		tileLights[tileIndex] = currentTile;
	}
}