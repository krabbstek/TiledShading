#version 430 core

in float viewSpaceDepth;

out float out_ViewSpaceDepth;

void main()
{
	out_ViewSpaceDepth = viewSpaceDepth;
}