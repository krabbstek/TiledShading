#version 450 core

in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;
in vec3 albedo;

out vec3 out_ViewSpaceNormal;
out vec3 out_ViewSpacePosition;
out vec3 out_Albedo;

void main()
{
	out_ViewSpacePosition = viewSpacePosition;
	out_ViewSpaceNormal = viewSpaceNormal;
	out_Albedo = albedo;
}