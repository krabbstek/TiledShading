#version 450 core

in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

out vec3 out_ViewSpacePosition;
out vec3 out_ViewSpaceNormal;

layout (binding = 3, r32i) uniform coherent iimage2D u_TileMinImage;
layout (binding = 4, r32i) uniform coherent iimage2D u_TileMaxImage;

uniform int u_TileSize;
uniform float u_FarDepth;

void main()
{
	out_ViewSpacePosition = viewSpacePosition;
	out_ViewSpaceNormal = viewSpaceNormal;

	ivec2 tileCoords = ivec2(gl_FragCoord.xy) / u_TileSize;

	int depth = int(viewSpacePosition.z * (float(0x7FFFFFFF) / u_FarDepth) + 0.5);
	imageAtomicMin(u_TileMinImage, tileCoords, depth);
	imageAtomicMax(u_TileMaxImage, tileCoords, -depth);
}