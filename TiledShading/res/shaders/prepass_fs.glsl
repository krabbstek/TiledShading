#version 450 core

in vec3 albedo;
in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

layout (location = 0) out vec3 out_Albedo;
layout (location = 1) out vec3 out_ViewSpacePosition;
layout (location = 2) out vec3 out_ViewSpaceNormal;

void main()
{
	out_Albedo = albedo;
	out_ViewSpacePosition = viewSpacePosition;
	out_ViewSpaceNormal = viewSpaceNormal;
}