#version 450 core

in vec4 in_Position;
in vec2 in_TexCoords;

out vec2 texCoords;

void main()
{
	gl_Position = in_Position;
	texCoords = in_TexCoords;
}