#include "TiledDeferredComputeLightTilesPass.h"

#include "Globals.h"

#include <algorithm>

TiledDeferredComputeLightTilesPass::TiledDeferredComputeLightTilesPass(
	Renderer& renderer,
	std::shared_ptr<GLShader> computeShader,
	std::shared_ptr<GLTexture2D> viewSpacePositionTexture,
	std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture,
	std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture,
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO)
	: RenderPass(renderer, computeShader),
	m_ViewSpacePositionTexture(viewSpacePositionTexture),
	m_TileMinDepthImageTexture(tileMinDepthImageTexture),
	m_TileMaxDepthImageTexture(tileMaxDepthImageTexture),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(&g_Lights, sizeof(g_Lights))),
	m_LightIndexSSBO(lightIndexSSBO)
{
	m_LeftPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.leftPlanes, sizeof(m_TileGrid.leftPlanes));
	m_RightPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.rightPlanes, sizeof(m_TileGrid.rightPlanes));
	m_BottomPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.bottomPlanes, sizeof(m_TileGrid.bottomPlanes));
	m_TopPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.topPlanes, sizeof(m_TileGrid.topPlanes));

	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
	m_Shader->SetUniform1f("u_FarPlaneDepth", g_FarPlaneDepth);

	memset(m_LightIndices, 0, sizeof(m_LightIndices));
	m_LightIndexSSBO->SetData(m_LightIndices, sizeof(m_LightIndices));
}

TiledDeferredComputeLightTilesPass::~TiledDeferredComputeLightTilesPass()
{
}


void TiledDeferredComputeLightTilesPass::Render(std::vector<Renderable*>& renderables)
{
#ifdef USE_COMPUTE_SHADER
	m_ViewSpacePositionTexture->Bind(0);
	
	m_LightSSBO->SetData(&g_Lights, sizeof(g_Lights));
	m_LightSSBO->Bind(3);

	m_LightIndexSSBO->Bind(4);

	m_LeftPlanesSSBO->Bind(10);
	m_RightPlanesSSBO->Bind(11);
	m_BottomPlanesSSBO->Bind(12);
	m_TopPlanesSSBO->Bind(13);
	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
	m_Shader->DispatchComputeShader(g_NumTileCols, g_NumTileRows, 1);

#else

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

	m_TileGrid.ComputeLightTiles((Light*)g_LightGrid, g_LightGridSize * g_LightGridSize, tileMinDepth, tileMaxDepth, *m_LightIndexSSBO);

#endif
}