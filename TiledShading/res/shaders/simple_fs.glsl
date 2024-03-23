#version 450 core

out vec4 out_Color;

uniform vec4 u_Color = vec4(0.2, 0.2, 0.8, 1.0);

void main()
{
	out_Color = u_Color;
}