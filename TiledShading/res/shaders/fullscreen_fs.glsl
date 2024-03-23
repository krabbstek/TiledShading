#version 450 core

in vec2 texCoords;

out vec4 out_Color;

layout (binding = 0) uniform sampler2D u_Texture;

void main()
{
	out_Color = texture(u_Texture, texCoords);
}