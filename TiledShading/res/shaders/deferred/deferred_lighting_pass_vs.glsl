#version 450 core

in vec4 in_Position;

void main()
{
	gl_Position = in_Position;
}