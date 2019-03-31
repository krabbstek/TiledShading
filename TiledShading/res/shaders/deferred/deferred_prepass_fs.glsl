#version 450 core

in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

out vec3 out_ViewSpacePosition;
out vec3 out_ViewSpaceNormal;

void main()
{
	out_ViewSpacePosition = viewSpacePosition;
	out_ViewSpaceNormal = viewSpaceNormal;
}