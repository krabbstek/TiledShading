#version 430 core

out vec4 out_Color;

void main()
{
	out_Color = vec4(vec3(1.0 - 0.04 * gl_FragCoord.z / gl_FragCoord.w), 1.0);
}