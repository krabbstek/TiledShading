#version 430 core

layout (location = 0) in vec4 in_Position;

out float viewSpaceDepth;

uniform mat4 u_MV = mat4(1.0);
uniform mat4 u_MVP = mat4(1.0);

void main()
{
	vec4 row = transpose(u_MV)[2];
	viewSpaceDepth = dot(row, in_Position);

	gl_Position = u_MVP * in_Position;
}