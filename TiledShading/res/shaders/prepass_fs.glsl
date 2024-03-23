#version 450 core

in vec3 albedo;
in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

layout (location = 0) out vec3 out_Albedo;
layout (location = 1) out vec3 out_ViewSpacePosition;
layout (location = 2) out vec3 out_ViewSpaceNormal;

layout (r32ui, binding = 0) uniform coherent uimage2D u_TileMinTexture;
layout (r32ui, binding = 1) uniform coherent uimage2D u_TileMaxTexture;

uniform int u_TileSize = 40;

void main()
{
	out_Albedo = albedo;
	out_ViewSpacePosition = viewSpacePosition;
	out_ViewSpaceNormal = normalize(viewSpaceNormal);

	ivec2 texCoord = ivec2(gl_FragCoord.xy / u_TileSize);
	//imageAtomicMin(u_TileMinTexture, texCoord, uint(float(0xFFFFFFFFu) * gl_FragDepth));
	//imageAtomicMax(u_TileMaxTexture, texCoord, uint(float(0xFFFFFFFFu) * gl_FragDepth));

	//imageStore(u_TileMinTexture, texCoord, ivec4(0x7FFFFFFFu));
	//imageStore(u_TileMaxTexture, texCoord, ivec4(0x7FFFFFFFu));
}