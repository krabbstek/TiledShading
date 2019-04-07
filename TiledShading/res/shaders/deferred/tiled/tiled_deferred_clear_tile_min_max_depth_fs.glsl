#version 430 core

layout (binding = 3, r32i) uniform writeonly iimage2D u_TileMinImage;
layout (binding = 4, r32i) uniform writeonly iimage2D u_TileMaxImage;

void main()
{
	ivec2 texelCoord = ivec2(gl_FragCoord.xy);
	imageStore(u_TileMinImage, texelCoord, ivec4(0x7FFFFFFF));
	imageStore(u_TileMaxImage, texelCoord, ivec4(-0x7FFFFFFF));
}