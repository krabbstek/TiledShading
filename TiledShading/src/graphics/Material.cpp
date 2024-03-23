#include "Material.h"

#include "Globals.h"

void Material::Bind(GLShader& shader) const
{
	shader.SetUniform4f("u_Material.albedo", albedo);
	shader.SetUniform1f("u_Material.reflectivity", reflectivity);
	shader.SetUniform1f("u_Material.shininess", shininess);
	shader.SetUniform1f("u_Material.metalness", metalness);
	shader.SetUniform1f("u_Material.fresnel", fresnel);
	shader.SetUniform1i("u_TileSize", g_TileSize);
	shader.SetUniform1i("u_NumTileCols", tileCols);
}