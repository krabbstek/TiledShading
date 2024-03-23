#version 430 core

out vec4 out_Color;

layout (binding = 0) uniform sampler2D u_TileTexture;

uniform int u_TileSize;

void main()
{
	float z = texelFetch(u_TileTexture, (ivec2(gl_FragCoord.xy) / u_TileSize), 0).r;
	out_Color = vec4(vec3(-z * 0.03), 1.0);
}