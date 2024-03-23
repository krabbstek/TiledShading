#version 430 core

out vec4 out_Color;

layout (binding = 0) uniform sampler2D u_Texture;

void main()
{
	out_Color = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
}