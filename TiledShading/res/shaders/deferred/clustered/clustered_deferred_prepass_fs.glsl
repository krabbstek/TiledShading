#version 430 core

in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

out vec3 out_ViewSpacePosition;
out vec3 out_ViewSpaceNormal;

uniform int u_TileSize;
uniform float u_FarDepth;

void main()
{
	out_ViewSpacePosition = viewSpacePosition;
	out_ViewSpaceNormal = viewSpaceNormal;
}