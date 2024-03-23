#include "TiledDeferredComputeLightTilesPass.h"

#include "Globals.h"

TiledDeferredComputeLightTilesPass::TiledDeferredComputeLightTilesPass(
	Renderer& renderer,
	std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture,
	std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture,
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO,
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO)
	: RenderPass(renderer, std::shared_ptr<GLShader>(nullptr)),
	m_TileMinDepthImageTexture(tileMinDepthImageTexture),
	m_TileMaxDepthImageTexture(tileMaxDepthImageTexture),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO)
{
}

TiledDeferredComputeLightTilesPass::~TiledDeferredComputeLightTilesPass()
{
}


void TiledDeferredComputeLightTilesPass::Render(std::vector<Renderable*>& renderables)
{
	int tilePixels[g_NumTileRows * g_NumTileCols];
	float tileMinDepth[g_NumTileRows * g_NumTileCols];
	float tileMaxDepth[g_NumTileRows * g_NumTileCols];

	m_TileMinDepthImageTexture->BindAsTexture();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, tilePixels));
	// Convert from ''int space'' back to view space depth
	for (int i = 0; i < g_NumTileRows * g_NumTileCols; i++)
		tileMinDepth[i] = float(tilePixels[i]) * (g_FarPlaneDepth / float(0x7FFFFFFF));

	m_TileMaxDepthImageTexture->BindAsTexture();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, tilePixels));
	// Convert from ''int space'' back to view space depth
	for (int i = 0; i < g_NumTileRows * g_NumTileCols; i++)
		tileMaxDepth[i] = float(tilePixels[i]) * (g_FarPlaneDepth / float(0x7FFFFFFF));

	m_TileGrid.ComputeLightTiles((Light*)g_LightGrid, g_LightGridSize * g_LightGridSize, tileMinDepth, tileMaxDepth, *m_LightIndexSSBO, *m_TileIndexSSBO);
}