#version 450 core

in float z;

out vec4 out_Color;

void main()
{
	out_Color = vec4(vec3(1.0 + 0.04 * z), 1.0);
}