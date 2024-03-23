#version 430 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec3 in_Albedo;

out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;
out vec3 albedo;

uniform mat4 u_MV = mat4(1.0);
uniform mat4 u_MVP = mat4(1.0);

uniform mat4 u_MV_normal = mat4(1.0);

void main()
{
	viewSpacePosition = (u_MV * in_Position).xyz;
	viewSpaceNormal = mat3(u_MV_normal) * in_Normal;
	albedo = in_Albedo;

	gl_Position = u_MVP * in_Position;
}