#version 450 core

layout (location = 0) in vec4 in_Position;

out noperspective float z;

uniform mat4 u_MV = mat4(1.0);
uniform mat4 u_MVP = mat4(1.0);

void main()
{
	gl_Position = u_MVP * in_Position;
	z = (u_MV * in_Position).z;
}